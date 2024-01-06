#pragma once
#include <Objects/Enemies/EnemyBase.h>
#include <GENERATED/ExplodingEnemy.h>
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
	void Update() override;
	void OnKilled() override;
};