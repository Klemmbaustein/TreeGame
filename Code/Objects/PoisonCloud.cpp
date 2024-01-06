#include "PoisonCloud.h"
#include <Engine/Stats.h>
#include <Objects/PlayerObject.h>

void PoisonCloud::Begin()
{
	Particle = new ParticleComponent();
	Attach(Particle);
	Particle->LoadParticle("PoisonCloud");
}

void PoisonCloud::Update()
{
	Time += Performance::DeltaTime;
	if (Time < 3)
	{
		if (PlayerObject::GetPlayer() && Vector3::Distance(PlayerObject::GetPlayer()->GetTransform().Location, GetTransform().Location) < 12)
		{
			PlayerObject::GetPlayer()->DealDamage(Performance::DeltaTime * 25);
		}
	}
	else
	{
		Particle->SetActive(false);
		if (Time > 4)
		{
			Objects::DestroyObject(this);
		}
	}
}
