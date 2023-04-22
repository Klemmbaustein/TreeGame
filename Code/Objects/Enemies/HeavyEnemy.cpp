#include "HeavyEnemy.h"
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

void HeavyEnemy::Begin()
{
	EnemyMesh = new MeshComponent();
	Attach(EnemyMesh);
	EnemyMesh->Load("HeavyDrone");
	Collider = new CollisionComponent();
	Attach(Collider);
	ModelGenerator::ModelData CollisionMesh;
	CollisionMesh.AddElement().MakeCube(2, 0);
	Collider->Init(CollisionMesh.GetMergedVertices(), CollisionMesh.GetMergedIndices());
	Health = 200;
	Value = 100;
}

void HeavyEnemy::Tick()
{
	if (IsInEditor) return;

	if (PlayerObject::GetPlayer() && Vector3::Distance(PlayerObject::GetPlayer()->GetTransform().Location, GetTransform().Location) < 50)
	{
		WeaponCooldown -= Performance::DeltaTime;
		if (WeaponCooldown <= 0)
		{
			WeaponCooldown = 0.5;
			BulletObject* NewBullet = Objects::SpawnObject<BulletObject>(GetTransform());
			NewBullet->Player = PlayerObject::GetPlayer();
			NewBullet->Direction = PlayerObject::GetPlayer()->GetTransform().Location - GetTransform().Location;
		}
	}
	if (TargetTree)
	{
		float TreeDistance = Vector3::Distance(TargetTree->GetTransform().Location + Vector3(0, 10, 0), GetTransform().Location);
		float Speed = std::max(TreeDistance - 150, 0.f) + 5;
		auto ColliderHit = dynamic_cast<PlayerObject*>(Collider->OverlapCheck().HitObject);
		if (ColliderHit)
		{
			ColliderHit->DealDamage(25 * Performance::DeltaTime);
		}
		DirectionOffset += Random::GetRandomFloat(-1.f, 1.f) * Performance::DeltaTime;
		Direction.Y += DirectionOffset * Performance::DeltaTime * 50;
		{
			Vector3 NewDir = ((TargetTree->GetTransform().Location + Vector3(0, 10, 0)) - GetTransform().Location).Normalize();
			Vector3 LerpedDirection = Vector3::Lerp(Vector3::GetForwardVector(Direction), NewDir, Performance::DeltaTime);
			Direction = Vector3::LookAtFunction(0, LerpedDirection);
		}
		GetTransform().Location += Vector3::GetForwardVector(Direction) * Performance::DeltaTime * Speed;
		GetTransform().Rotation = Direction + Vector3(0, -90, 0);
		if (TreeDistance < 10)
		{
			TargetTree->Health -= 200 * Performance::DeltaTime;
		}
	}
	else
	{
		TargetTree = GetTargetTree();
		Direction = Vector3::LookAtFunction(GetTransform().Location, TargetTree->GetTransform().Location + Vector3(0, 10, 0));
	}
}

void HeavyEnemy::OnKilled()
{
	Sound::PlaySound3D(Enemies::ExplosionSound, GetTransform().Location, 1, 1, 35);
	CameraShake::PlayDefaultCameraShake(0.5);
	Objects::SpawnObject<ParticleObject>(GetTransform())->LoadParticle("DroneExplosion");
	Objects::DestroyObject(this);
}
