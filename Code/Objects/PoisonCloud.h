#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/PoisonCloud.h>
#include <Objects/Components/ParticleComponent.h>

class PoisonCloud : public WorldObject
{
	ParticleComponent* Particle = nullptr;
	float Time = 0;
public:
	POISONCLOUD_GENERATED("Game");
	void Begin() override;
	void Update() override;
};