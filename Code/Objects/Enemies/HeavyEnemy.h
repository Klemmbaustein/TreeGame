#pragma once
#include <GENERATED/GENERATED_HeavyEnemy.h>
#include <Objects/Enemies/EnemyBase.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class HeavyEnemy : public EnemyBase
{
	CollisionComponent* Collider = nullptr;
	MeshComponent* EnemyMesh = nullptr;
	Vector3 Direction;
	float DirectionOffset = 0;
	float WeaponCooldown = 0.3;
public:
	HEAVYENEMY_GENERATED("Game/Enemies");
	void Begin() override;
	void Tick() override;
	void OnKilled() override;
};