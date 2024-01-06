#include "TurretObject.h"
#include <Objects/Enemies/EnemyBase.h>
#include <Sound/Sound.h>
#include <Engine/Stats.h>
#include <Objects/PlayerObject.h>
#include <Engine/Log.h>

namespace Turret
{
	Sound::SoundBuffer* TurretSound = nullptr;
}

void TurretObject::OnLevelUp()
{
	if (!TurretMesh)
	{
		TurretMesh = new MeshComponent();
		Attach(TurretMesh);
		TurretMesh->Load("Turret");
		TurretMesh->RelativeTransform.Location.Y = 1;
		TurretMesh->RelativeTransform.Scale = 1.25;

		TurretBarrel = new MeshComponent();
		Attach(TurretBarrel);
		TurretBarrel->Load("TurretBarrel");
		TurretBarrel->RelativeTransform.Location.Y = 6;
		TurretBarrel->RelativeTransform.Scale = 1.25;
	}
}

void TurretObject::Begin()
{
	if (!Turret::TurretSound)
	{
		Turret::TurretSound = Sound::LoadSound("Turret");
	}

	TurretBase = new MeshComponent();
	Attach(TurretBase);
	TurretBase->Load("TurretBase");
	TurretCollision = new CollisionComponent();
	Attach(TurretCollision);
	TurretCollision->Load(TurretBase->GetModelData().GetMergedVertices(), TurretBase->GetModelData().GetMergedIndices());

	ShootParticle = new ParticleComponent();
	Attach(ShootParticle);
	ShootParticle->LoadParticle("TurretShoot");
	ShootParticle->SetActive(false);
}

void TurretObject::Update()
{
	// If the turret isn't built (Level == 0), we don't do anything.
	if (!Level) return;
	Cooldown -= Performance::DeltaTime;

	// If the turret's cooldown is not greater than 0, we search for an enemy near the turret.
	if (Cooldown > 0) return;
	TargetedEnemy = nullptr;
	for (WorldObject* o : Objects::AllObjects)
	{
		if (dynamic_cast<EnemyBase*>(o) && Vector3::Distance(o->GetTransform().Location, GetTransform().Location) < Radius)
		{
			TargetedEnemy = dynamic_cast<EnemyBase*>(o);
		}
	}
	if (!TargetedEnemy) return;

	// Then we damage an enemy near the turret, play a shoot sound and rotate the turret towards the enemy.
	Cooldown = std::clamp(0.5f - (Level / 10.f), 0.f, 5.f) + 0.1;
	TurretMesh->RelativeTransform.Rotation.Y
		= Vector3::LookAtFunction(GetTransform().Location, TargetedEnemy->GetTransform().Location).Y + 90;
	Vector3 Rot = Vector3::LookAtFunction(GetTransform().Location + Vector3(0, 2, 0), TargetedEnemy->GetTransform().Location) + Vector3(0, 90, 0);
	Vector3 NewRot = Vector3(Rot.Z, Rot.Y, Rot.X);

	TurretBarrel->RelativeTransform.Rotation = NewRot;

	ShootParticle->RelativeTransform.Location = TurretBarrel->RelativeTransform.Location + Vector3::GetForwardVector(Rot - Vector3(0, -90, 0)) * Vector3(-8, 8, -8);
	ShootParticle->RelativeTransform.Rotation = Vector3::LookAtFunctionY(ShootParticle->RelativeTransform.Location + GetTransform().Location,
		TargetedEnemy->GetTransform().Location);
	ShootParticle->Reset();	
	ShootParticle->SetActive(true);

	TargetedEnemy->Health -= 8;
	if (TargetedEnemy->Health <= 0)
	{
		PlayerObject::GetPlayer()->OnMoneyAdded(TargetedEnemy->Value);
	}
	TargetedEnemy->OnDamaged();
	Sound::PlaySound3D(Turret::TurretSound, GetTransform().Location, 1, 1, 10);
}

bool TurretObject::Buy(uint64_t& Money)
{
	if (Level < 5 && Money >= (800 + Level * 400))
	{
		Money -= 800 + Level * 400;
		Level++;
		OnLevelUp();
		return true;
	}
	return false;
}

TurretObject::ShopStatus TurretObject::GetShopStatus()
{
	if (Level == 0)
	{
		return ShopStatus(800, "Buy turret (Press [E])");
	}
	if (Level < 5)
	{
		return ShopStatus(800 + Level * 400, "Upgrade turret to level " + std::to_string(Level + 1) + " (Press [E])");
	}
	return ShopStatus(0, "Turret is at max level.");
}