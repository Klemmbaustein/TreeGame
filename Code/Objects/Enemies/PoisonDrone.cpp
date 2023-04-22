#include "PoisonDrone.h"
#include <Objects/TreeObject.h>
#include <World/Stats.h>
#include <Rendering/Camera/CameraShake.h>
#include <Objects/PlayerObject.h>
#include <Objects/ParticleObject.h>
#include <Engine/EngineRandom.h>
#include <Objects/PoisonCloud.h>

namespace Enemies
{
	extern Sound::SoundBuffer* ExplosionSound;
}

void PoisonDrone::Begin()
{
	if (!Enemies::ExplosionSound)
	{
		Enemies::ExplosionSound = Sound::LoadSound("Explosion");
	}
	EnemyMesh = new MeshComponent();
	Attach(EnemyMesh);
	EnemyMesh->Load("PoisonDrone");
	Collider = new CollisionComponent();
	Attach(Collider);
	ModelGenerator::ModelData CollisionMesh;
	CollisionMesh.AddElement().MakeCube(2, 0);
	Collider->Init(CollisionMesh.GetMergedVertices(), CollisionMesh.GetMergedIndices());
	Collider->CollMesh.CanOverlap = false;
	Health = 15;
	Value = 30;
}

void PoisonDrone::Tick()
{
	if (IsInEditor) return;
	EnemyMesh->GetRelativeTransform().Location.Y = sin(Stats::Time) / 3;
	if (TargetTree)
	{
		float TreeDistance = Vector3::Distance(TargetTree->GetTransform().Location + Vector3(0, 10, 0), GetTransform().Location);
		float Speed = std::max(TreeDistance - 150, 0.f) + 10;
		auto ColliderHit = dynamic_cast<PlayerObject*>(Collider->OverlapCheck().HitObject);
		if (ColliderHit)
		{
			ColliderHit->DealDamage(50);
			OnKilled();
		}
		DirectionOffset += Random::GetRandomFloat(-1.f, 1.f) * Performance::DeltaTime;
		Direction.Y += DirectionOffset * Performance::DeltaTime * 25;
		{
			Vector3 NewDir = ((TargetTree->GetTransform().Location + Vector3(0, 10, 0)) - GetTransform().Location).Normalize();
			Vector3 LerpedDirection = Vector3::Lerp(Vector3::GetForwardVector(Direction), NewDir, Performance::DeltaTime);
			Direction = Vector3::LookAtFunction(0, LerpedDirection);
		}
		GetTransform().Location += Vector3::GetForwardVector(Direction) * Performance::DeltaTime * Speed;
		GetTransform().Rotation = Direction + Vector3(0, -90, 0);
		if (Speed < 5)
		{
			TargetTree->Health -= 100;
			OnKilled();
		}
	}
	else
	{
		TargetTree = GetTargetTree();
		Direction = Vector3::LookAtFunction(GetTransform().Location, TargetTree->GetTransform().Location + Vector3(0, 10, 0));
	}
}

void PoisonDrone::OnKilled()
{
	Transform PoisonTransform = GetTransform();
	PoisonTransform.Location.Y = 0;
	Objects::SpawnObject<PoisonCloud>(PoisonTransform);
	Sound::PlaySound3D(Enemies::ExplosionSound, GetTransform().Location, 1, 1, 35);
	CameraShake::PlayDefaultCameraShake(0.5);
	Objects::SpawnObject<ParticleObject>(GetTransform())->LoadParticle("DroneExplosion");
	Objects::DestroyObject(this);
}
