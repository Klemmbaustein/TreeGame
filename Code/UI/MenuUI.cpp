#include "MenuUI.h"
#include <UI/UIBackground.h>
#include <UI/UIButton.h>
#include <UI/UIText.h>
#include <Engine/Scene.h>
#include <Engine/Application.h>
#include <Engine/Save.h>
#include <Engine/Log.h>

void MenuUI::GenerateSettignsMenu()
{
	SettingsBackgrounds[0]->DeleteChildren();

	MenuItem GraphicsSettings[NumSettings] =
	{
		MenuItem("VSync       : " + std::string(Graphics::VSync ? "on " : "off"), 3),
		MenuItem("Fullscreen  : " + std::string(Application::GetFullScreen() ? "on " : "off"), 4),
		MenuItem("SSAO        : " + std::string(Graphics::SSAO ? "on " : "off"), 5),
		MenuItem("Shadows     : " + std::string(Graphics::RenderShadows ? "on " : "off"), 6),
		MenuItem("Bloom       : " + std::string(Graphics::Bloom ? "on " : "off"), 7),
	};

	SettingsBackgrounds[0]->AddChild(new UIText(1, 1, "Settings", Text));

	size_t i = 0;
	for (const MenuItem& m : GraphicsSettings)
	{
		UIButton* Button = new UIButton(false, 0, Vector3(0.5, 1, 0.5), this, m.Index);
		Button->SetMinSize(Vector2(0.3, 0.1));
		Button->SetBorder(UIBox::E_ROUNDED, 1);
		Button->SetOpacity(0.9);
		UIText* ButtonText = new UIText(0.75, 0, m.Name, Text);
		Button->AddChild(ButtonText);
		SettingsBackgrounds[0]->AddChild(Button);
		i++;
	}
}

void MenuUI::SaveSettings()
{
	SaveGame SettingsSave = SaveGame("Graphics");

	SettingsSave.SetPropterty(SaveGame::SaveProperty("VSync", std::to_string(Graphics::VSync), Type::E_INT));
	SettingsSave.SetPropterty(SaveGame::SaveProperty("Fullscreen", std::to_string(Application::GetFullScreen()), Type::E_INT));
	SettingsSave.SetPropterty(SaveGame::SaveProperty("SSAO", std::to_string(Graphics::SSAO), Type::E_INT));
	SettingsSave.SetPropterty(SaveGame::SaveProperty("Shadows", std::to_string(Graphics::RenderShadows), Type::E_INT));
	SettingsSave.SetPropterty(SaveGame::SaveProperty("Bloom", std::to_string(Graphics::Bloom), Type::E_INT));
}

void MenuUI::LoadSettings()
{
	HighScore = 0;
	SaveGame GameplaySave = SaveGame("Gameplay");
	if (GameplaySave.GetPropterty("Highscore").Type != Type::E_NULL)
	{
		HighScore = std::stoi(GameplaySave.GetPropterty("Highscore").Value);
	}

	try
	{
		SaveGame SettingsSave = SaveGame("Graphics");
		Graphics::VSync = std::stoi(SettingsSave.GetPropterty("VSync").Value);
		Application::SetFullScreen(std::stoi(SettingsSave.GetPropterty("Fullscreen").Value));
		Graphics::SSAO = std::stoi(SettingsSave.GetPropterty("SSAO").Value);
		Graphics::RenderShadows = std::stoi(SettingsSave.GetPropterty("Shadows").Value);
		Graphics::Bloom = std::stoi(SettingsSave.GetPropterty("Bloom").Value);

	}
	catch (std::exception& e)
	{
		Log::Print("Error loading settings from file 'Settings.save': " + std::string(e.what()));
	}
}

MenuUI::MenuUI()
{
	LoadSettings();

	Text = new TextRenderer("Font.ttf", 150);

	UIBackground* MenuBackground = new UIBackground(false, Vector2(-0.85, -0.55), Vector3(0.1), Vector2(0.0, 0.5));
	MenuBackground->SetBorder(UIBox::E_ROUNDED, 1);
	MenuBackground->SetOpacity(0.8);
	MenuBackground->Align = UIBox::E_REVERSE;

	MenuItem Menu[3] =
	{
		MenuItem("Play", 0),
		MenuItem("Settings", 1),
		MenuItem("Quit", 2)
	};

	for (const MenuItem& m : Menu)
	{
		UIButton* Button = new UIButton(false, 0, Vector3(0.5, 1, 0.5), this, m.Index);
		Button->SetMinSize(Vector2(0.3, 0.1));
		Button->SetBorder(UIBox::E_ROUNDED, 1);
		Button->SetOpacity(0.9);
		UIText* ButtonText = new UIText(0.75, 0, m.Name, Text);
		Button->AddChild(ButtonText);
		MenuBackground->AddChild(Button);
	}

	SettingsBackgrounds[0] = new UIBackground(false, Vector2(-0.5, -0.95), Vector3(0.1), Vector2(0.0, 0.9));
	SettingsBackgrounds[0]->SetBorder(UIBox::E_ROUNDED, 1);
	SettingsBackgrounds[0]->SetOpacity(0.8);
	SettingsBackgrounds[0]->Align = UIBox::E_REVERSE;
	SettingsBackgrounds[0]->IsVisible = false;

	SettingsBackgrounds[1] = new UIBackground(false, Vector2(-0.1, -0.95), Vector3(0.1), Vector2(0.4, 0.9));
	SettingsBackgrounds[1]->SetBorder(UIBox::E_ROUNDED, 1);
	SettingsBackgrounds[1]->SetOpacity(0.8);
	SettingsBackgrounds[1]->Align = UIBox::E_REVERSE;
	SettingsBackgrounds[1]->IsVisible = false;
	SettingsBackgrounds[1]->AddChild((new UIText(1, 1, "Stats:", Text)));
	SettingsBackgrounds[1]->AddChild((new UIText(0.75, 1, "High score: " + std::to_string(HighScore) + " Waves", Text)));

	GenerateSettignsMenu();
}

MenuUI::~MenuUI()
{
	delete Text;
}

void MenuUI::Tick()
{
}

void MenuUI::OnButtonClicked(int Index)
{
	if (Index == 0)
	{
		Scene::LoadNewScene("DesertMap");
		SaveSettings();
	}
	if (Index == 1)
	{
		for (auto* bg : SettingsBackgrounds)
		{
			bg->IsVisible = !bg->IsVisible;
		}
		UIBox::RedrawUI();
	}
	if (Index == 2)
	{
		Application::Quit();
		SaveSettings();
	}
	if (Index >= 3 && SettingsBackgrounds[0]->IsVisible)
	{
		void (*Settings[NumSettings])() =
		{
			[]() { Graphics::VSync = !Graphics::VSync; },
			[]() { Application::SetFullScreen(!Application::GetFullScreen()); },
			[]() { Graphics::SSAO = !Graphics::SSAO; },
			[]() { Graphics::RenderShadows = !Graphics::RenderShadows; },
			[]() { Graphics::Bloom = !Graphics::Bloom; }
		};

		Settings[Index - 3]();
		GenerateSettignsMenu();
	}
}
