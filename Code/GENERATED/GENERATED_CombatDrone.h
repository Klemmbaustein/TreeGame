#pragma once
#define COMBATDRONE_GENERATED(Category) CombatDrone() : WorldObject(ObjectDescription("CombatDrone", 1036166389)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 1036166389;}