#pragma once
#include <Objects/BuyableBase.h>
#include <GENERATED/GENERATED_WeaponShop.h>
#include <Weapon.h>
class WeaponShop : public BuyableBase
{
public:

	WEAPONSHOP_GENERATED("Game/Buyable");
	void Begin() override;
	uint8_t WeaponLevel = 0;
	bool Buy(uint64_t& Money) override;
	ShopStatus GetShopStatus() override;
};