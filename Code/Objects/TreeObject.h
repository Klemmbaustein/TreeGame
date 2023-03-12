#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/GENERATED_TreeObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

class TreeObject : public WorldObject
{
	MeshComponent* TreeMesh = nullptr;
	CollisionComponent* Collider = nullptr;
public:
	bool IsDead = false;
	const float MaxHealth = 1000;
	float Health = MaxHealth;
	TREEOBJECT_GENERATED("Game");
	void Begin() override;
	void Tick() override;
	void Destroy() override;
};