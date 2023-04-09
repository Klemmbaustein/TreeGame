#include "WeaponShop.h"
#include <Objects/Components/MeshComponent.h>
#include <Objects/Components/CollisionComponent.h>
#include <Objects/PlayerObject.h>

void WeaponShop::Begin()
{
	MeshComponent* ShopMesh = new MeshComponent();
	Attach(ShopMesh);
	ShopMesh->Load("WeaponStation");
	ShopMesh->GetRelativeTransform().Scale = 0.8;
	CollisionComponent* Collider = new CollisionComponent();
	Attach(Collider);
	Collider->Init(ShopMesh->GetModelData().GetMergedVertices(), ShopMesh->GetModelData().GetMergedIndices());
	Collider->RelativeTransform.Scale = 1;
}

#define WEAPON_LEVEL_UP(Type, Price) if (Money >= Price)\
{\
	Money -= Price;\
	PlayerObject::GetPlayer()->SetWeapon(Type());\
	WeaponLevel++;\
	return true;\
}\
return false;

bool WeaponShop::Buy(uint64_t& Money)
{
	switch (WeaponLevel)
	{
	case 0:
		WEAPON_LEVEL_UP(SMG, 300);
	case 1:
		WEAPON_LEVEL_UP(Rifle, 1000);
	case 2:
		WEAPON_LEVEL_UP(Sniper, 1500);
	default:
		return false;
	}
}

WeaponShop::ShopStatus WeaponShop::GetShopStatus()
{
	switch (WeaponLevel)
	{
	case 0:
		return ShopStatus(300, "Buy SMG (Press [E])");
	case 1:
		return ShopStatus(1000, "Buy Rifle (Press [E])");
	case 2:
		return ShopStatus(1500, "Buy Sniper (Press [E])");
	default:
		return ShopStatus(0, "Weapon is at max level");
		break;
	}
}
