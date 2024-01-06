#pragma once
#include <GENERATED/RadarTower.h>
#include <Objects/Buyable/BuyableBase.h>

class RadarTower : public BuyableBase
{
public:
	int8_t Level = 0;
	RADARTOWER_GENERATED("Game/Buyable");
	
	void Begin() override;

	bool Buy(uint64_t& Money) override;
	ShopStatus GetShopStatus() override;
};