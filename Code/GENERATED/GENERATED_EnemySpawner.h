#pragma once
#define ENEMYSPAWNER_GENERATED(Category) EnemySpawner() : WorldObject(ObjectDescription("EnemySpawner", 41004473)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 41004473;}