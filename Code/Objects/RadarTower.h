#pragma once
#include <GENERATED/GENERATED_RadarTower.h>
#include <Objects/BuyableBase.h>

class RadarTower : public BuyableBase
{
public:
	int8_t Level = 0;
	RADARTOWER_GENERATED("Game/Buyable");
	
	void Begin() override;

	bool Buy(uint64_t& Money) override;
	ShopStatus GetShopStatus() override;
};