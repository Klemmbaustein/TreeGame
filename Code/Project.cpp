#include <string>

// This is the name of the current project
char ProjectName[] = "TreeGame";

// This function returns the map that will be loaded when the game is opened
std::string GetStartupScene()
{
#if EDITOR
	return "DesertMap";
#endif
	return "Menu";
}