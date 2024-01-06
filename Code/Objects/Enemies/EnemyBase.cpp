#include "EnemyBase.h"
#include <Objects/TreeObject.h>

TreeObject* EnemyBase::GetTargetTree()
{
	auto AllTrees = Objects::GetAllObjectsWithID(TreeObject::GetID());
	if(AllTrees.size())
		return dynamic_cast<TreeObject*>(AllTrees[0]);
	return nullptr;
}

void EnemyBase::OnDamaged()
{
	if (Health <= 0)
	{
		OnKilled();
	}
}

void EnemyBase::OnKilled()
{
}

void EnemyBase::Begin()
{
}

void EnemyBase::Update()
{
}

void EnemyBase::Destroy()
{
}
