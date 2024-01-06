#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/EnemyBase.h>

class TreeObject;

class EnemyBase : public virtual WorldObject
{
public:
	static TreeObject* GetTargetTree();
	TreeObject* TargetTree = nullptr;
	ENEMYBASE_GENERATED("Game/Enemies");
	float Health = 100.f;
	uint64_t Value = 10;
	virtual void OnDamaged();
	virtual void OnKilled();
	virtual void Begin() override;
	virtual void Update() override;
	virtual void Destroy() override;
};