#pragma once
#include <UI/Default/UICanvas.h>

class TextRenderer;
class UIButton;
class UIBackground;
constexpr int NumSettings = 5;

class MenuUI : public UICanvas
{
	struct MenuItem
	{
		std::string Name;
		int Index;
	};
	UIBackground* SettingsBackgrounds[2];
	TextRenderer* Text;
public:
	void GenerateSettignsMenu();
	void SaveSettings();
	void LoadSettings();
	MenuUI();
	~MenuUI();
	void Tick() override;
	void OnButtonClicked(int Index) override;
};