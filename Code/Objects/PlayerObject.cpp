#include "PlayerObject.h"
#include <Engine/Input.h>
#include <World/Stats.h>
#include <Objects/ParticleObject.h>
#include <Rendering/Camera/CameraShake.h>
#include <Engine/Log.h>
#include <UI/GameUI.h>
#include <Engine/EngineRandom.h>
#include <Objects/Enemies/EnemyBase.h>
#include <Objects/Buyable/BuyableBase.h>
#include <Engine/Save.h>
#include <Engine/Scene.h>

PlayerObject* CurrentPlayer = nullptr;

void PlayerObject::OnMoneyAdded(uint64_t Amount)
{
	Money += Amount;
	RecentlyAddedMoney += Amount;
	MoneyTimer = 3;
}

PlayerObject* PlayerObject::GetPlayer()
{
	return CurrentPlayer;
}

void PlayerObject::DealDamage(float Amount)
{
	if (Health <= 0) return;
	if ((Health - Amount) <= 0 && Health > 0)
	{
		Log::Print("Player died at wave " + std::to_string(CurrentWave + 1), Log::LogColor::Red);
		DisplayedUI->ShowMessage("You died at wave " + std::to_string(CurrentWave + 1), Vector3(1, 0.5, 0));
		QuitGameToMenu();
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
	WeaponMesh->GetRelativeTransform().Scale = 0.2;
}

void PlayerObject::Begin()
{
	Input::CursorVisible = false;
	Movement = new MoveComponent();
	Attach(Movement);
	Movement->Velocity = 900;
	Movement->JumpHeight = 50;
	Movement->Gravity = 200;
	PlayerCamera = new CameraComponent();
	Attach(PlayerCamera);
	PlayerCamera->Use();
	PlayerCamera->GetTransform().Location.Y = 1.5;
	PlayerCamera->SetFOV(M_PI / 1.2);
	SetWeapon(Pistol());
	DisplayedUI = UICanvas::CreateNewCanvas<GameUI>();
	if (DisplayedUI) DisplayedUI->Player = this;
	HitSound = Sound::LoadSound("Hit");
	CurrentPlayer = this;
	UpgradeSound = Sound::LoadSound("Upgrade");
	for (uint8_t i = 0; i < 4; i++)
	{
		Footsteps[i] = Sound::LoadSound("Footstep" + std::to_string(i + 1));
	}
}

void PlayerObject::Tick()
{
	// If any of these conditions are met, we shouldn't tick the player
	if (IsInEditor) return;
	if (Health <= 0)
	{
		if (PlayerDeathTimer.TimeSinceCreation() > 5)
		{
			Scene::LoadNewScene("Menu");
		}
		return;
	}

	// Play footstep sounds if the footstep timer wasn't reset in 0.4 seconds and the player is moving on ground
	if (FootstepTimer.TimeSinceCreation() > 0.4 && Movement->GetVelocity().Length() > 15 && Movement->GetIsOnGround())
	{
		Sound::PlaySound2D(Footsteps[Random::GetRandomNumber(0, 4)], 1.5, 0.45);
		FootstepTimer.Reset();
	}

	// Handle input
	Vector3 ForwardDirection;
	ForwardDirection.Y = PlayerCamera->GetTransform().Rotation.Y;
	if (Input::IsKeyDown(SDLK_w) || Input::IsKeyDown(SDLK_UP))
	{
		Movement->AddMovementInput(Vector3::GetForwardVector(ForwardDirection));
	}
	if (Input::IsKeyDown(SDLK_s) || Input::IsKeyDown(SDLK_DOWN))
	{
		Movement->AddMovementInput(-Vector3::GetForwardVector(ForwardDirection));
	}
	if (Input::IsKeyDown(SDLK_d) || Input::IsKeyDown(SDLK_RIGHT))
	{
		Movement->AddMovementInput(Vector3::GetRightVector(ForwardDirection));
	}
	if (Input::IsKeyDown(SDLK_a) || Input::IsKeyDown(SDLK_LEFT))
	{
		Movement->AddMovementInput(-Vector3::GetRightVector(ForwardDirection));
	}
	if (Input::IsKeyDown(SDLK_SPACE))
	{		
		// Play a jump sound
		if (Movement->GetIsOnGround() && FootstepTimer.TimeSinceCreation() > 0.1)
		{
			Sound::PlaySound2D(Footsteps[Random::GetRandomNumber(0, 4)], 1.5, 0.35);
			FootstepTimer.Reset();
		}
		Movement->Jump();
	}
	if (Input::IsKeyDown(SDLK_r))
	{
		//TODO: Reload (?)
	}
	// Handle camera input. The X rotation (Pitch) is clamped to -89° and +89° to avoid the camera going upside-down.
	PlayerCamera->GetTransform().Rotation.Y += Input::MouseMovement.X * 1.25f;
	PlayerCamera->GetTransform().Rotation.X += Input::MouseMovement.Y * 1.25f;
	PlayerCamera->GetTransform().Rotation.X = std::clamp(PlayerCamera->GetTransform().Rotation.X, -89.f, 89.f);



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
		GetTransform().Location + Vector3(0, 1.5, 0) + Vector3::GetForwardVector(PlayerCamera->GetTransform().Rotation) * 15,
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
	if ((Input::IsKeyDown(SDLK_e) || Input::IsKeyDown(SDLK_RETURN)) && !IsEDown)
	{
		IsEDown = true;
		if (Shop && Shop->Buy(Money))
		{
			Sound::PlaySound2D(UpgradeSound);
			CameraShake::PlayDefaultCameraShake(1.5);
		}
	}
	else if (!(Input::IsKeyDown(SDLK_e) || Input::IsKeyDown(SDLK_RETURN))) IsEDown = false;


	// Handle logic for weapons
	if (ShootCooldown == 0)
	{
		// If RecentlyHit = false, the crosshair will not be red.
		// To give hit feedback, the crosshair will turn red until the weapon can shoot again.
		RecentlyHit = false;
	}
	if (Input::IsLMBDown && ShootCooldown == 0)
	{
		Sound::PlaySound2D(CurrentWeapon.Sound, Random::GetRandomNumber(0.9f, 1.1f), CurrentWeapon.SoundVolume);
		ShootCooldown = CurrentWeapon.Cooldown;
		CameraShake::PlayDefaultCameraShake(CurrentWeapon.Cooldown * 1.5);
		auto ShootHit = Collision::LineTrace(GetTransform().Location + Vector3(0, 1.5, 0),
			GetTransform().Location + Vector3(0, 1.5, 0) + Vector3::GetForwardVector(PlayerCamera->GetTransform().Rotation) * 2000000,
			{this});
		PlayerCamera->GetTransform().Rotation.X += CurrentWeapon.Knockback * 2;
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

	// Tick the money counter next to the crosshair
	if (MoneyTimer > 0)
	{
		MoneyTimer -= Performance::DeltaTime;
		if (MoneyTimer <= 0)
		{
			RecentlyAddedMoney = 0;
		}
	}

	// Do some very suspicous math to translate the weapon in front of the player camera
	Vector3 WeaponLocation = Vector3(0, 65, 0) - Vector3(Movement->GetVelocity().X, -Movement->GetVelocity().Y, Movement->GetVelocity().Z) / 8;
	WeaponLocation += Vector3::GetForwardVector(PlayerCamera->GetTransform().Rotation) * (65 - pow(ShootCooldown * 10, 2) * 5);
	WeaponLocation += Vector3::GetRightVector(PlayerCamera->GetTransform().Rotation) * 45;
	WeaponLocation += Vector3::GetUpVector(PlayerCamera->GetTransform().Rotation) * (-35);

	WeaponMesh->GetRelativeTransform().Location = WeaponLocation / 50;
	Vector3 OffsetRotation = PlayerCamera->GetTransform().Rotation;

	OffsetRotation.X += pow(ShootCooldown * 10, 2) * 4;
	WeaponMesh->GetRelativeTransform().Rotation = OffsetRotation;
	WeaponMesh->GetRelativeTransform().Rotation.Y += 90;
	float ZRot = WeaponMesh->GetRelativeTransform().Rotation.Z;
	WeaponMesh->GetRelativeTransform().Rotation.Z = WeaponMesh->GetRelativeTransform().Rotation.X;
	WeaponMesh->GetRelativeTransform().Rotation.X = ZRot;

}

void PlayerObject::Destroy()
{
	// Delete all sounds created by the player object.
	delete HitSound;
	delete UpgradeSound;
	for (auto& s : Footsteps)
	{
		delete s;
	}
}

void PlayerObject::QuitGameToMenu()
{
	PlayerDeathTimer.Reset();
	Health = 0;
	SaveGame GameplaySave = SaveGame("Gameplay");
	try
	{
		if (GameplaySave.GetPropterty("Highscore").Type != Type::E_NULL)
		{
			int CurrentHighscore = std::stoi(GameplaySave.GetPropterty("Highscore").Value);
			if ((CurrentWave + 1) > CurrentHighscore)
			{
				GameplaySave.SetPropterty(SaveGame::SaveProperty("Highscore", std::to_string(CurrentWave + 1), Type::E_INT));
			}
		}
		else
		{
			GameplaySave.SetPropterty(SaveGame::SaveProperty("Highscore", std::to_string(CurrentWave + 1), Type::E_INT));
		}
	}
	catch (std::exception&)
	{

	}
}
