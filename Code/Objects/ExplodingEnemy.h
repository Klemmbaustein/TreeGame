#pragma once
#include <Objects/EnemyBase.h>
#include <GENERATED/GENERATED_ExplodingEnemy.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class ExplodingEnemy : public EnemyBase
{
	CollisionComponent* Collider = nullptr;
	MeshComponent* EnemyMesh = nullptr;
	Vector3 Direction;
	float DirectionOffset = 0;
public:
	EXPLODINGENEMY_GENERATED("Game/Enemies");
	void Begin() override;
	void Tick() override;
	void OnKilled() override;
};