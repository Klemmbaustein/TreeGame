#pragma once
#define TREEOBJECT_GENERATED(Category) TreeObject() : WorldObject(ObjectDescription("TreeObject", 3009091690)) {}\
static std::string GetCategory() { return Category; }\
static uint32_t GetID() { return 3009091690;}