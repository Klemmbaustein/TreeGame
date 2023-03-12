#pragma once
#define WEAPONCOLLECTIBLE_GENERATED(Category) WeaponCollectible() : WorldObject(ObjectDescription("WeaponCollectible", 1405188405)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 1405188405;}