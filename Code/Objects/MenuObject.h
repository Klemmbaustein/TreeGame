#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/MenuObject.h>

class MenuObject : public WorldObject
{
public:
	MENUOBJECT_GENERATED("Game/Menu");
	void Begin() override;
	void Update() override;
	void Destroy() override;
};