#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/GENERATED_BulletObject.h>

class PlayerObject;

class BulletObject : public WorldObject
{
public:
	PlayerObject* Player = nullptr;
	BULLETOBJECT_GENERATED("Game/Enemies");
	Vector3 Direction;
	float Speed = 2;
	float Lifetime = 20;

	void Begin() override;
	void Tick() override;
};