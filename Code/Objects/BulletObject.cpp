#include "BulletObject.h"
#include <World/Stats.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/PlayerObject.h>

void BulletObject::Begin()
{
	MeshComponent* BulletMesh = new MeshComponent();
	Attach(BulletMesh);
	BulletMesh->Load("Bullet");
	BulletMesh->GetRelativeTransform().Scale = 0.2;
}

void BulletObject::Tick()
{
	GetTransform().Location += Direction * Speed * Performance::DeltaTime;
	if (Player && Vector3::Distance(GetTransform().Location, Player->GetTransform().Location) < 2)
	{
		Player->DealDamage(10);
		Objects::DestroyObject(this);
	}
	Lifetime -= Performance::DeltaTime;
	if (Lifetime < 0) Objects::DestroyObject(this);
}
