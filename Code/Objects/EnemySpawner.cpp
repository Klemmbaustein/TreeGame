#include "EnemySpawner.h"
#include <World/Stats.h>
#include <Objects/ExplodingEnemy.h>
#include <Objects/HeavyEnemy.h>
#include <Objects/PlayerObject.h>
#include <Objects/CombatDrone.h>
#include <Objects/PoisonDrone.h>
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
	Properties.push_back(Objects::Property("Enemy ID", Type::E_INT, &EnemyID));
	auto PlayerObjects = Objects::GetAllObjectsWithID(PlayerObject::GetID());
	if (PlayerObjects.size())
	{
		Player = (PlayerObject*)PlayerObjects[0];
	}
}

void EnemySpawner::Tick()
{
	if (IsInEditor) return;
	if (Player && Player->WaveActive && (Player->CurrentWave / 2) >= EnemyID)
	{
		SpawnTime -= Performance::DeltaTime;
		if (SpawnTime <= 0)
		{
			float Time = std::clamp(Enemies::SpawnTimes[EnemyID] - Player->CurrentWave * 1.5f, 3.f, 60.f);
			SpawnTime = Random::GetRandomNumber(Time, Time + 3);
			Objects::SpawnObjectFromID(Enemies::EnemyIDs[EnemyID], Transform(GetTransform().Location + Vector3(0, 10, 0), Vector3(0), Vector3(1)));
		}
	}
}
