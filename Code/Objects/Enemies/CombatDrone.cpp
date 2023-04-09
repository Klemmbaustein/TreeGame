#include "CombatDrone.h"
#include <Objects/TreeObject.h>
#include <World/Stats.h>
#include <Rendering/Camera/CameraShake.h>
#include <Objects/PlayerObject.h>
#include <Objects/ParticleObject.h>
#include <Engine/EngineRandom.h>
#include <Objects/BulletObject.h>
#include <Engine/Log.h>

namespace Enemies
{
	extern Sound::SoundBuffer* ExplosionSound;
}

void CombatDrone::Begin()
{
	EnemyMesh = new MeshComponent();
	Attach(EnemyMesh);
	EnemyMesh->Load("CombatDrone");
	Collider = new CollisionComponent();
	Attach(Collider);
	ModelGenerator::ModelData CollisionMesh;
	CollisionMesh.MakeCube(2, 0, 0);
	Collider->Init(CollisionMesh.GetMergedVertices(), CollisionMesh.GetMergedIndices());
	Collider->CollMesh.CanOverlap = false;
	Health = 50;
	Value = 25;
}

void CombatDrone::Tick()
{
	if (IsInEditor) return;
	if (Vector3::Distance(PlayerObject::GetPlayer()->GetTransform().Location, GetTransform().Location) < 50)
	{
		WeaponCooldown -= Performance::DeltaTime;
		if (WeaponCooldown <= 0)
		{
			WeaponCooldown = 0.3;
			BulletObject* NewBullet = Objects::SpawnObject<BulletObject>(GetTransform());
			NewBullet->Player = PlayerObject::GetPlayer();
			NewBullet->Direction = PlayerObject::GetPlayer()->GetTransform().Location - GetTransform().Location;
		}
	}
	if (TargetTree)
	{
		float TreeDistance = Vector3::Distance(TargetTree->GetTransform().Location + Vector3(0, 10, 0), GetTransform().Location);
		float Speed = std::max(TreeDistance - 150, 0.f) + 10;
		auto ColliderHit = dynamic_cast<PlayerObject*>(Collider->OverlapCheck().HitObject);
		if (ColliderHit)
		{
			ColliderHit->DealDamage(25 * Performance::DeltaTime);
		}
		DirectionOffset += Random::GetRandomFloat(-1.f, 1.f) * Performance::DeltaTime;
		Direction.Y += DirectionOffset * Performance::DeltaTime * 75;
		{
			Vector3 NewDir = ((TargetTree->GetTransform().Location + Vector3(0, 10, 0)) - GetTransform().Location).Normalize();
			Vector3 LerpedDirection = Vector3::Lerp(Vector3::GetForwardVector(Direction), NewDir, Performance::DeltaTime);
			Direction = Vector3::LookAtFunction(0, LerpedDirection);
		}
		GetTransform().Location += Vector3::GetForwardVector(Direction) * Performance::DeltaTime * Speed;
		GetTransform().Rotation = Direction + Vector3(0, -90, 0);
		if (TreeDistance < 10)
		{
			TargetTree->Health -= 50 * Performance::DeltaTime;
		}
	}
	else
	{
		TargetTree = GetTargetTree();
		Direction = Vector3::LookAtFunction(GetTransform().Location, TargetTree->GetTransform().Location + Vector3(0, 10, 0));
	}
}

void CombatDrone::OnKilled()
{
	Sound::PlaySound3D(Enemies::ExplosionSound, GetTransform().Location, 1, 1, 35);
	CameraShake::PlayDefaultCameraShake(0.5);
	Objects::SpawnObject<ParticleObject>(GetTransform())->LoadParticle("DroneExplosion");
	Objects::DestroyObject(this);
}
