#include "PlayerObject.h"
#include <Engine/Input.h>
#include <Engine/Stats.h>
#include <Objects/ParticleObject.h>
#include <Objects/Components/ParticleComponent.h>
#include <Rendering/Camera/CameraShake.h>
#include <Engine/Log.h>
#include <UI/GameUI.h>
#include <Engine/EngineRandom.h>
#include <Objects/Enemies/EnemyBase.h>
#include <Objects/Buyable/BuyableBase.h>
#include <Engine/File/Save.h>
#include <Engine/Scene.h>
#include <Engine/Utility/FileUtility.h>
#include <Engine/Console.h>

Vector3 PlayerObject::TranslateToWeaponLocation(Vector3 in)
{	
	// Do some very suspicous math to translate the weapon in front of the player camera
	Vector3 WeaponLocation = Vector3(0, 65, 0) - Vector3(Movement->GetVelocity().X, -Movement->GetVelocity().Y, Movement->GetVelocity().Z) / 8;
	WeaponLocation += Vector3::GetForwardVector(PlayerCamera->RelativeTransform.Rotation) * (in.Z - pow(ShootCooldown * 10, 2) * 5);
	WeaponLocation += Vector3::GetRightVector(PlayerCamera->RelativeTransform.Rotation) * in.X;
	WeaponLocation += Vector3::GetUpVector(PlayerCamera->RelativeTransform.Rotation) * (-in.Y);
	return WeaponLocation / 50;
}

void PlayerObject::OnMoneyAdded(uint64_t Amount)
{
	Money += Amount;
	RecentlyAddedMoney += Amount;
	MoneyTimer = 3;
}

PlayerObject* CurrentPlayer = nullptr;
PlayerObject* PlayerObject::GetPlayer()
{
	return CurrentPlayer;
}

void PlayerObject::DealDamage(float Amount)
{
	if (Godmode) return;
	if (Health <= 0) return;
	if ((Health - Amount) <= 0 && Health > 0)
	{
		Log::Print("Player died at wave " + std::to_string(CurrentWave + 1), Log::LogColor::Red);
		DisplayedUI->ShowMessage("You died at wave " + std::to_string(CurrentWave + 1), Vector3(1, 0.2, 0));
		Health -= Amount;
		QuitGameToMenu();
		CameraShake::PlayDefaultCameraShake(5);
		return;
	}
	Health -= Amount;
	CameraShake::PlayDefaultCameraShake(Amount / 10);
}

void PlayerObject::SetWeapon(Weapon NewWeapon)
{
	if (WeaponMesh)
	{
		Detach(WeaponMesh);
	}
	CurrentWeapon = NewWeapon;
	WeaponMesh = new MeshComponent();
	Attach(WeaponMesh);
	WeaponMesh->Load(CurrentWeapon.Mesh);
	WeaponMesh->RelativeTransform.Scale = 0.2;
}

void PlayerObject::Begin()
{
	CurrentPlayer = this;


	Input::CursorVisible = false;
	Movement = new MoveComponent();
	Attach(Movement);
	Movement->Velocity = 900;
	Movement->JumpHeight = 50;
	Movement->Gravity = 200;

	PlayerCamera = new CameraComponent();
	Attach(PlayerCamera);
	PlayerCamera->Use();
	PlayerCamera->RelativeTransform.Location.Y = 1.5;
	PlayerCamera->SetFOV(80);

	MuzzleFlash = new ParticleComponent();
	Attach(MuzzleFlash);
	MuzzleFlash->LoadParticle("Shoot");

	SetWeapon(Pistol());

	DisplayedUI = UICanvas::CreateNewCanvas<GameUI>();
	if (DisplayedUI) DisplayedUI->Player = this;

	HitSound = Sound::LoadSound("Hit");
	UpgradeSound = Sound::LoadSound("Upgrade");
	for (uint8_t i = 0; i < 4; i++)
	{
		Footsteps[i] = Sound::LoadSound("Footstep" + std::to_string(i + 1));
	}
	WaterSound = Sound::LoadSound("WaterSplash");
	
	Console::RegisterCommand(Console::Command("givemoney", []()
		{
			if (PlayerObject::GetPlayer())
			{
				PlayerObject::GetPlayer()->OnMoneyAdded(std::stoll(Console::CommandArgs()[0]));
				Log::Print("Added money to player");
			}
		},
	{ Console::Command::Argument("amount", Type::Int) }));

	Console::RegisterCommand(Console::Command("god", []()
		{
			if (PlayerObject::GetPlayer())
			{
				Log::Print(PlayerObject::GetPlayer()->Godmode ? "Godmode off" : "Godmode on");
				PlayerObject::GetPlayer()->Godmode = !PlayerObject::GetPlayer()->Godmode;
			}
		},
		{ }));

	Console::RegisterCommand(Console::Command("setwave", []()
		{
			if (PlayerObject::GetPlayer())
			{
				Log::Print("Set wave to " + Console::CommandArgs()[0]);
				PlayerObject::GetPlayer()->CurrentWave = std::stoi(Console::CommandArgs()[0]);
			}
		},
		{ Console::Command::Argument("wave", Type::Int) }));
}

void PlayerObject::Update()
{
	// If any of these conditions are met, we shouldn't tick the player
	if (IsInEditor)
		return;

	if (Health <= 0)
	{
		if (PlayerDeathTimer.Get() > 5)
		{
			Scene::LoadNewScene("Menu");
		}
		return;
	}

	// If the player is in the water of the water map, we kill the player.
	if (FileUtil::GetFileNameWithoutExtensionFromPath(CurrentScene) == "WaterMap" && GetTransform().Location.Y < -2.5)
	{
		Sound::PlaySound2D(WaterSound, 0.7, 1);
		DealDamage(Health + 100);
		if (Godmode)
		{
			GetTransform().Location = Vector3(0, 10, 0);
		}
		else
		{
			DisplayedUI->ScreenOverlay->SetColor(Vector3(0.1, 0.2, 0.3));
			Movement->Active = false;
		}
	}

	// Play footstep sounds if the footstep timer wasn't reset in 0.4 seconds and the player is moving on ground
	if (FootstepTimer.Get() > 0.4 && Movement->GetVelocity().Length() > 15 && Movement->GetIsOnGround())
	{
		Sound::PlaySound2D(Footsteps[Random::GetRandomInt(0, 4)], 1.5, 0.45);
		FootstepTimer.Reset();
	}

	// Handle input
	Vector3 ForwardDirection;
	ForwardDirection.Y = PlayerCamera->RelativeTransform.Rotation.Y;
	if (Input::IsKeyDown(Input::Key::w) || Input::IsKeyDown(Input::Key::UP))
	{
		Movement->AddMovementInput(Vector3::GetForwardVector(ForwardDirection));
	}
	if (Input::IsKeyDown(Input::Key::s) || Input::IsKeyDown(Input::Key::DOWN))
	{
		Movement->AddMovementInput(-Vector3::GetForwardVector(ForwardDirection));
	}
	if (Input::IsKeyDown(Input::Key::d) || Input::IsKeyDown(Input::Key::RIGHT))
	{
		Movement->AddMovementInput(Vector3::GetRightVector(ForwardDirection));
	}
	if (Input::IsKeyDown(Input::Key::a) || Input::IsKeyDown(Input::Key::LEFT))
	{
		Movement->AddMovementInput(-Vector3::GetRightVector(ForwardDirection));
	}
	if (Input::IsKeyDown(Input::Key::SPACE))
	{		
		// Play a jump sound
		if (Movement->GetIsOnGround() && FootstepTimer.Get() > 0.1)
		{
			Sound::PlaySound2D(Footsteps[Random::GetRandomInt(0, 4)], 1.5, 0.35);
			FootstepTimer.Reset();
		}
		Movement->Jump();
	}
	// Handle camera input. The X rotation (Pitch) is clamped to -89� and +89� to avoid the camera going upside-down.
	PlayerCamera->RelativeTransform.Rotation.Y += Input::MouseMovement.X * 1.25f;
	PlayerCamera->RelativeTransform.Rotation.X += Input::MouseMovement.Y * 1.25f;
	PlayerCamera->RelativeTransform.Rotation.X = std::clamp(PlayerCamera->RelativeTransform.Rotation.X, -89.f, 89.f);



	// Handle wave logic (which is obviously in the player)
	WaveTimer += Performance::DeltaTime;
	if (WaveTimer > 0)
	{
		if (WaveTimer > 20)
		{
			if (!WaveActive && WaveTimer < 60)
			{
				DisplayedUI->ShowMessage("Wave " + std::to_string(CurrentWave + 1), Vector3(1, 0.5, 0));
			}
			WaveActive = true;
			if (WaveTimer > 60)
			{
				bool WaveEnded = true;
				for (WorldObject* o : Objects::AllObjects)
				{
					if (dynamic_cast<EnemyBase*>(o)) WaveEnded = false;
				}
				// if there is an alive enemy on the map, don't end the wave. However we shouldn't spawn any new enemies as well.
				if (!WaveEnded)
				{
					WaveActive = false;
					WaveTimer = 60;
				}
				else
				{
					DisplayedUI->ShowMessage("Wave " + std::to_string(CurrentWave + 1) + " completed!", Vector3(0, 1, 0));
					Log::Print("Wave " + std::to_string(CurrentWave + 1) + " has ended", Log::LogColor::Green);
					WaveActive = false;
					WaveTimer = 0;
					CurrentWave++;
					Health = 100.f;
				}
			}
		}
		else WaveActive = false;
	}

	// Check for buyable objects and interact if necessary.
	auto ForwardHit = Collision::LineTrace(GetTransform().Location + Vector3(0, 1.5, 0),
		GetTransform().Location + Vector3(0, 1.5, 0) + Vector3::GetForwardVector(PlayerCamera->RelativeTransform.Rotation) * 15,
		{ this });
	BuyableBase* Shop = dynamic_cast<BuyableBase*>(ForwardHit.HitObject);
	if (Shop)
	{
		BuyableBase::ShopStatus Transaction = Shop->GetShopStatus();
		TransactionString = Transaction.NextName;
		TransactionMoney = Transaction.NextValue;
	}
	else if (TransactionString.size() || TransactionMoney)
	{
		TransactionString.clear();
		TransactionMoney = 0;
	}
	// Interact with the shop if the player presses 'E' or 'enter' (Return)
	if ((Input::IsKeyDown(Input::Key::e) || Input::IsKeyDown(Input::Key::RETURN)) && !IsEDown)
	{
		IsEDown = true;
		if (Shop && Shop->Buy(Money))
		{
			Sound::PlaySound2D(UpgradeSound);
			CameraShake::PlayDefaultCameraShake(1.5);
		}
	}
	else if (!(Input::IsKeyDown(Input::Key::e) || Input::IsKeyDown(Input::Key::RETURN))) IsEDown = false;


	// Handle logic for weapons
	if (ShootCooldown == 0)
	{
		// If RecentlyHit = false, the crosshair will not be red.
		// To give hit feedback, the crosshair will turn red until the weapon can shoot again.
		RecentlyHit = false;
	}
	if (Input::IsLMBDown && ShootCooldown == 0)
	{
		MuzzleFlash->Reset();

		MuzzleFlash->RelativeTransform.Location = TranslateToWeaponLocation(Vector3(45, 35, 60) + CurrentWeapon.MuzzleFlashOffset);

		Sound::PlaySound2D(CurrentWeapon.Sound, Random::GetRandomFloat(0.9f, 1.1f), CurrentWeapon.SoundVolume);
		ShootCooldown = CurrentWeapon.Cooldown;
		CameraShake::PlayDefaultCameraShake(CurrentWeapon.Cooldown * 1.5);
		auto ShootHit = Collision::LineTrace(GetTransform().Location + Vector3(0, 1.5, 0),
			GetTransform().Location + Vector3(0, 1.5, 0) + Vector3::GetForwardVector(PlayerCamera->RelativeTransform.Rotation) * 2000000,
			{this});
		PlayerCamera->RelativeTransform.Rotation.X += CurrentWeapon.Knockback * 2;
		PlayerCamera->RelativeTransform.Rotation.Y += CurrentWeapon.Knockback * std::sin(Stats::Time * 8) * 2;
		if (ShootHit.Hit)
		{
			auto Obj = Objects::SpawnObject<ParticleObject>(Transform(ShootHit.ImpactPoint, Vector3::LookAtFunctionY(0, ShootHit.Normal), 1));
			Obj->LoadParticle("Impact");
			EnemyBase* HitEnemy = dynamic_cast<EnemyBase*>(ShootHit.HitObject);
			if (HitEnemy)
			{
				HitEnemy->Health -= CurrentWeapon.Damage;
				HitEnemy->OnDamaged();
				if (HitEnemy->Health <= 0)
				{
					OnMoneyAdded(HitEnemy->Value);
				}
				RecentlyHit = true;
				Sound::PlaySound2D(HitSound, 1, 0.75);
				CameraShake::PlayDefaultCameraShake(CurrentWeapon.Cooldown * 1);
			}
		}
	}

	ShootCooldown = std::max(ShootCooldown - Performance::DeltaTime, 0.f);

	// Update the money counter next to the crosshair
	if (MoneyTimer > 0)
	{
		MoneyTimer -= Performance::DeltaTime;
		if (MoneyTimer <= 0)
		{
			RecentlyAddedMoney = 0;
		}
	}


	Vector3 WeaponLocation = TranslateToWeaponLocation(Vector3(45, 35, 60));
	MuzzleFlash->RelativeTransform.Location = Vector3::LookAtFunctionY(0, Vector3::GetForwardVector(PlayerCamera->RelativeTransform.Rotation));

	WeaponMesh->RelativeTransform.Location = WeaponLocation;
	Vector3 OffsetRotation = PlayerCamera->RelativeTransform.Rotation;

	OffsetRotation.X += pow(ShootCooldown * 10, 2) * 4;
	WeaponMesh->RelativeTransform.Rotation = OffsetRotation;
	WeaponMesh->RelativeTransform.Rotation.Y += 90;
	float ZRot = WeaponMesh->RelativeTransform.Rotation.Z;
	WeaponMesh->RelativeTransform.Rotation.Z = WeaponMesh->RelativeTransform.Rotation.X;
	WeaponMesh->RelativeTransform.Rotation.X = ZRot;
}

void PlayerObject::Destroy()
{
	CurrentPlayer = nullptr;
	// Delete all sounds created by the player object.
	if (HitSound)
	{
		delete HitSound;
		delete UpgradeSound;
		if (WaterSound)
		{
			delete WaterSound;
		}
		for (auto& s : Footsteps)
		{
			delete s;
		}
	}
}

void PlayerObject::QuitGameToMenu()
{
	PlayerDeathTimer.Reset();
	if (Health > 0)
	{
		Health = 0;
	}
	SaveGame GameplaySave = SaveGame("Gameplay");
	try
	{
		if (GameplaySave.GetProperty("Highscore").Type != Type::Int)
		{
			int CurrentHighscore = std::stoi(GameplaySave.GetProperty("Highscore").Value);
			if ((CurrentWave + 1) > CurrentHighscore)
			{
				GameplaySave.SetProperty(SaveGame::SaveProperty("Highscore", std::to_string(CurrentWave + 1), Type::Int));
			}
		}
		else
		{
			GameplaySave.SetProperty(SaveGame::SaveProperty("Highscore", std::to_string(CurrentWave + 1), Type::Int));
		}
	}
	catch (std::exception&)
	{

	}
}
