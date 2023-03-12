#include "RadarTower.h"
#include <Objects/TurretObject.h>
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>

void RadarTower::Begin()
{
	MeshComponent* m = new MeshComponent();
	Attach(m);
	m->Load("RadarTower");
	CollisionComponent* c = new CollisionComponent();
	Attach(c);
	c->Init(m->GetModelData().GetMergedVertices(), m->GetModelData().GetMergedIndices());
}

bool RadarTower::Buy(uint64_t& Money)
{
	if (Money < 1500) return false;
	if (Level > 2) return false;
	Money -= 1500;
	for (WorldObject* o : Objects::AllObjects)
	{
		TurretObject* Turret = dynamic_cast<TurretObject*>(o);
		if (!Turret) continue;
		Turret->Radius += 20;
	}
	Level++;
	return true;
}

BuyableBase::ShopStatus RadarTower::GetShopStatus()
{
	return Level > 2 ? ShopStatus(0, "Maximum turret radius reached") : ShopStatus(1500, "Increase turret radius ("
		+ std::to_string(Level * 10 + 20) + "m -> " + std::to_string(Level * 10 + 30) + "m) (Press [E])");
}