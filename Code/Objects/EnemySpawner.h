#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/GENERATED_EnemySpawner.h>
#include <Engine/EngineRandom.h>

class PlayerObject;

class EnemySpawner : public WorldObject
{
	float SpawnTime = Random::GetRandomNumber(5, 10);
public:
	PlayerObject* Player = nullptr;
	ENEMYSPAWNER_GENERATED("Game/EnemySpawner");
	void Begin() override;
	void Tick() override;
	int EnemyID = 0;

};