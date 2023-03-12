#pragma once
#include <Objects/Enemies/EnemyBase.h>
#include <GENERATED/GENERATED_PoisonDrone.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class PoisonDrone : public EnemyBase
{
	CollisionComponent* Collider = nullptr;
	MeshComponent* EnemyMesh = nullptr;
	Vector3 Direction;
	float DirectionOffset = 0;
public:
	POISONDRONE_GENERATED("Game/Enemies");
	void Begin() override;
	void Tick() override;
	void OnKilled() override;
};