#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/BuyableBase.h>

// To have other WorldObjects derive from another WorldObject the inheritance needs to be virtual.
// Why? Because I can't be bothered making the build tool good. Thats why.
class BuyableBase : public virtual WorldObject
{
public:
	BUYABLEBASE_GENERATED("Game/Buyable");
	struct ShopStatus
	{
		uint64_t NextValue;
		std::string NextName;
	};

	virtual bool Buy(uint64_t& Money);
	virtual ShopStatus GetShopStatus();
};