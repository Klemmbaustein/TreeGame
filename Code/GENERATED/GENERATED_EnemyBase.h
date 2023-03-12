#pragma once
#define ENEMYBASE_GENERATED(Category) EnemyBase() : WorldObject(ObjectDescription("EnemyBase", 2079390038)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 2079390038;}