#include "EnemySpawner.h"
#include <Engine/Stats.h>
#include <Objects/Enemies/ExplodingEnemy.h>
#include <Objects/Enemies/HeavyEnemy.h>
#include <Objects/PlayerObject.h>
#include <Objects/Enemies/CombatDrone.h>
#include <Objects/Enemies/PoisonDrone.h>
#include <Engine/Log.h>

namespace Enemies
{
	uint32_t EnemyIDs[] =
	{
		ExplodingEnemy::GetID(),
		HeavyEnemy::GetID(),
		CombatDrone::GetID(),
		PoisonDrone::GetID()
	};
	float SpawnTimes[]
	{
		10,
		45,
		18,
		25
	};
}

void EnemySpawner::Begin()
{
	Properties.push_back(Property("Enemy ID", Type::Int, &EnemyID));
	auto PlayerObjects = Objects::GetAllObjectsWithID(PlayerObject::GetID());
	if (PlayerObjects.size())
	{
		Player = (PlayerObject*)PlayerObjects[0];
	}
}

void EnemySpawner::Update()
{
	if (IsInEditor) return;
	if (Player && Player->WaveActive && (Player->CurrentWave / 2) >= EnemyID)
	{
		SpawnTime -= Performance::DeltaTime;
		if (SpawnTime <= 0)
		{
			float Time = std::clamp(Enemies::SpawnTimes[EnemyID] - Player->CurrentWave * 1.5f, 3.f, 60.f);
			SpawnTime = Random::GetRandomFloat(Time, Time + 3);
			Objects::SpawnObjectFromID(Enemies::EnemyIDs[EnemyID], Transform(GetTransform().Location + Vector3(0, 10, 0), Vector3(0), Vector3(1)));
		}
	}
}

void EnemySpawner::OnPropertySet()
{
	Name = "EnemySpawner ID " + std::to_string(EnemyID);
}
