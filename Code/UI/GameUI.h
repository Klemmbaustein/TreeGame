#pragma once
#include <UI/Default/UICanvas.h>
#include <UI/UIBackground.h>
#include <UI/UIText.h>

class PlayerObject;
class TreeObject;

class GameUI : public UICanvas
{
	float MessageOpacity = 0.0f;
	bool MessageFadingIn = true; // if false, the message is fading out. if true, its fading in.
	bool IsMessageActive = false;
	struct MessageUI
	{
		UIBackground* MessageBackground;
		UIText* Message;
	}; MessageUI m;
	void TickMessage();
public:
	void ShowMessage(std::string Text, Vector3 Color);
	UIBackground* ScreenOverlay;
	TreeObject* Tree = nullptr;
	PlayerObject* Player;
	GameUI();
	~GameUI();
	void Tick() override;
	TextRenderer* Text;
	UIText* WaveText;
	UIText* TransactionTitle;
	UIText* TreeHealthText;
	UIText* PlayerHealthText;
	UIText* PlayerMoneyText;
	UIText* TransactionText;
	UIText* MoneyNotification;
	UIBox* CrosshairContainer;
	UIBackground* TreeHealthWarning;
	UIBackground* Crosshair;
};