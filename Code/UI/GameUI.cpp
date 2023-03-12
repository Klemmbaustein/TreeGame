#include "GameUI.h"
#include <UI/UIBackground.h>
#include <World/Assets.h>
#include <Rendering/Texture/Texture.h>
#include <Objects/PlayerObject.h>
#include <Objects/TreeObject.h>
#include <Objects/EnemyBase.h>
#include <Engine/Log.h>

void GameUI::TickMessage()
{
	if (!IsMessageActive) return;
	if (MessageFadingIn)
	{
		MessageOpacity += Performance::DeltaTime * 4;
		if (MessageOpacity > 8)
		{
			MessageFadingIn = false;
		}
	}
	else
	{
		MessageOpacity -= Performance::DeltaTime * 4;
		if (MessageOpacity <= 0)
		{
			IsMessageActive = false;
		}
	}
	float Dist = std::clamp(MessageOpacity / 3, 0.f, 0.35f);
	m.Message->SetOpacity(std::min(Dist * 4, 1.f));
	m.MessageBackground->SetOpacity(Dist * 2.5);
	m.MessageBackground->SetMinSize(Vector2(2, Dist));
	m.MessageBackground->SetPosition(Vector2(-1, 0.6 - Dist / 2.f));
	m.Message->SetPadding(0);
}

void GameUI::ShowMessage(std::string Text, Vector3 Color)
{
	MessageOpacity = 0.0f;
	MessageFadingIn = true;
	IsMessageActive = true;
	m.Message->SetOpacity(0);
	m.Message->SetText(Text);
	m.Message->SetColor(Color);
}

GameUI::GameUI()
{
	Text = new TextRenderer("Font.ttf", 60.f);
	this->Player = Player;

	ScreenOverlay = new UIBackground(true, -1, Vector3(1, 0, 0), 2);
	ScreenOverlay->SetOpacity(0);

	CrosshairContainer = new UIBox(true, Vector2(-0.05, -0.05 * Graphics::AspectRatio));

	Crosshair = new UIBackground(true, 0, Vector3(1), Vector2(0.1, 0.1 * Graphics::AspectRatio));
	Crosshair->SetPadding(0);
	Crosshair->SetUseTexture(true, Texture::LoadTexture("Crosshair"));
	CrosshairContainer->AddChild(Crosshair);

	MoneyNotification = new UIText(0.8, 1, " +15$", Text);
	MoneyNotification->SetPadding(0);
	CrosshairContainer->AddChild(MoneyNotification);
	UIBackground* PlayerStatusBackground = new UIBackground(false, Vector2(-0.975), 0.075);
	UIBackground* WaveBackground = new UIBackground(false, Vector2(-0.35, 0.7), 0.075, Vector2(0.7, 0.25));

	PlayerStatusBackground->SetOpacity(0.8);
	WaveBackground->SetOpacity(0.8);

	UIBox* WaveTextBackgrounds[2] =
	{
		new UIBox(true, 0),
		new UIBox(true, 0)
	};
	WaveBackground->SetBorder(UIBox::E_ROUNDED, 1);
	PlayerStatusBackground->SetBorder(UIBox::E_ROUNDED, 1);
	for (auto i : WaveTextBackgrounds)
	{
		i->SetMinSize(Vector2(0.7, 0.1));
		i->SetPadding(0);
		i->Align = UIBox::E_CENTERED;
		WaveBackground->AddChild(i);
	}


	PlayerHealthText = new UIText(1, 1, "Player: 100%", Text);
	PlayerStatusBackground->AddChild(PlayerHealthText);

	UIBox* PlayerMoneyBar = new UIBox(true, 0);
	PlayerMoneyBar->SetPadding(0);

	PlayerMoneyText = new UIText(1, 1, "Money: 0", Text);
	PlayerMoneyBar->AddChild(PlayerMoneyText);
	TransactionText = new UIText(1, Vector3(1, 0.1, 0), "-0", Text);
	TransactionText->IsVisible = false;
	PlayerMoneyBar->AddChild(TransactionText);

	PlayerStatusBackground->AddChild(PlayerMoneyBar);

	TransactionTitle = new UIText(0.8, Vector3(1, 0.1, 0), "Buy ", Text);
	PlayerStatusBackground->AddChild(TransactionTitle);

	WaveText = new UIText(1.25, Vector3(1, 1, 0), "Wave 1: 0%", Text);

	UIBox* TreeBox = new UIBox(true, 0);
	TreeBox->SetPadding(0);

	UIBackground* TreeIcon = new UIBackground(true, 0, 1, Vector2(0.075));
	TreeIcon->SetSizeMode(UIBox::E_PIXEL_RELATIVE);
	TreeIcon->SetUseTexture(true, Texture::LoadTexture("TreeIcon"));
	TreeBox->AddChild(TreeIcon);

	TreeHealthText = new UIText(1, 1, "Tree: 100%", Text);
	TreeBox->AddChild(TreeHealthText);

	TreeHealthWarning = new UIBackground(true, 0, 1, Vector2(0.075));
	TreeHealthWarning->SetSizeMode(UIBox::E_PIXEL_RELATIVE);
	TreeHealthWarning->SetUseTexture(true, Texture::LoadTexture("Warning"));
	TreeHealthWarning->SetOpacity(0);
	TreeBox->AddChild(TreeHealthWarning);

	m.MessageBackground = new UIBackground(true, Vector2(-1, 0.4), 0.075, Vector2(2, 0.35));
	m.MessageBackground->Align = UIBox::E_CENTERED;
	m.MessageBackground->SetOpacity(0);
	m.Message = new UIText(1.75, 1, "", Text);
	m.MessageBackground->AddChild(m.Message);

	WaveTextBackgrounds[0]->AddChild(TreeBox);
	WaveTextBackgrounds[1]->AddChild(WaveText);
}

GameUI::~GameUI()
{
}

void GameUI::Tick()
{
	TickMessage();
	bool CanBuyTransaction = ((int64_t)Player->Money - (int64_t)Player->TransactionMoney) >= 0;
	float CrosshairSize = 0.05 + pow(Player->ShootCooldown * 0.1 + 1, 2) - 1;
	CrosshairContainer->SetPosition(Vector2(-CrosshairSize, -CrosshairSize * Graphics::AspectRatio) * 0.5);
	Crosshair->SetMinSize(Vector2(CrosshairSize, CrosshairSize * Graphics::AspectRatio));
	Crosshair->SetColor(Player->RecentlyHit ? Vector3(1, 0.2, 0) : Vector3(1));
	ScreenOverlay->SetOpacity((1 - Player->Health / 100) / 1.25);

	TreeHealthWarning->SetOpacity(std::clamp(sin(Stats::Time * 4) + 1, 0.f, 1.f) * Player->EnemyNearTree);

	PlayerHealthText->SetText("Player: " + std::to_string((int)std::ceil(Player->Health)) + "%");
	PlayerMoneyText->SetText("Money: " + std::to_string(Player->Money) + "$");
	TransactionText->SetText("-" + std::to_string(Player->TransactionMoney) + "$");
	MoneyNotification->SetText(" +" + std::to_string(Player->RecentlyAddedMoney) + "$");
	MoneyNotification->IsVisible = Player->MoneyTimer > 0;

	if (Player->WaveTimer > 20)
	{
		WaveText->SetText("Wave " + std::to_string(Player->CurrentWave + 1) + ": " + std::to_string((int)((Player->WaveTimer - 20.f) / 40.f * 100)) + "%");
	}
	else
	{
		WaveText->SetText("Wave " + std::to_string(Player->CurrentWave + 1) + ": Preperation - " + std::to_string((int)(Player->WaveTimer / 20.f * 100)) + "%");
	}

	TransactionText->SetColor(CanBuyTransaction ? Vector3(0.1, 1, 0) : Vector3(1, 0.1, 0));
	TransactionTitle->SetColor(CanBuyTransaction ? Vector3(0.1, 1, 0) : Vector3(1, 0.1, 0));
	TransactionText->IsVisible = Player->TransactionMoney;
	TransactionTitle->IsVisible = !Player->TransactionString.empty();
	TransactionTitle->SetText(Player->TransactionString);
	
	if (!Tree)
	{
		Tree = EnemyBase::GetTargetTree();
		if (!Tree) return;
	}
	TreeHealthText->SetText("Tree: " + std::to_string((int)(Tree->Health / Tree->MaxHealth * 100)) + "%");
	
}
