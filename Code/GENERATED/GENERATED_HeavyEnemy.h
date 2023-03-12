#pragma once
#define HEAVYENEMY_GENERATED(Category) HeavyEnemy() : WorldObject(ObjectDescription("HeavyEnemy", 3100458364)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 3100458364;}