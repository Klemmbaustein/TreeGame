#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/GENERATED_MenuObject.h>

class MenuObject : public WorldObject
{
public:
	MENUOBJECT_GENERATED("Game/Menu");
	void Begin() override;
	void Tick() override;
	void Destroy() override;
};