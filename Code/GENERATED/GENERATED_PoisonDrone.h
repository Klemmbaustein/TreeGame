#pragma once
#define POISONDRONE_GENERATED(Category) PoisonDrone() : WorldObject(ObjectDescription("PoisonDrone", 3701515717)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 3701515717;}