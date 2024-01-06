#include <string>

namespace Project
{
	bool UseNetworkFunctions = false;

	// This is the name of the current project
	const char* ProjectName = "TreeGame";

	// This function returns the map that will be loaded when the game is opened
	std::string GetStartupScene()
	{
		return "Menu";
	}
	
	// This function will be called on startup.
	void OnLaunch()
	{
	}
}