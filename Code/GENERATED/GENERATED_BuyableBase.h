#pragma once
#define BUYABLEBASE_GENERATED(Category) BuyableBase() : WorldObject(ObjectDescription("BuyableBase", 1322004186)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 1322004186;}