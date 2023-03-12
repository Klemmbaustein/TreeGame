#pragma once
#include <Objects/Buyable/BuyableBase.h>
#include <GENERATED/GENERATED_TurretObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <Objects/Components/ParticleComponent.h>

class EnemyBase;

class TurretObject : public BuyableBase
{
	EnemyBase* TargetedEnemy = nullptr;
	MeshComponent* TurretBase = nullptr;
	MeshComponent* TurretMesh = nullptr;
	MeshComponent* TurretBarrel = nullptr;
	CollisionComponent* TurretCollision = nullptr;
	ParticleComponent* ShootParticle = nullptr;
	float Cooldown = 2;
	void OnLevelUp();
public:
	float Radius = 40;
	TURRETOBJECT_GENERATED("Game/Buyable");
	void Begin() override;
	void Tick() override;
	uint8_t Level = 0;
	bool Buy(uint64_t& Money) override;
	ShopStatus GetShopStatus() override;
};