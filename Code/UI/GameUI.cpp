#include "GameUI.h"
#include <UI/UIBackground.h>
#include <Engine/File/Assets.h>
#include <Rendering/Texture/Texture.h>
#include <Objects/PlayerObject.h>
#include <Objects/TreeObject.h>
#include <Objects/Enemies/EnemyBase.h>
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
	m.MessageBackground->SetMinSize(Vector2(2, 0));
	m.MessageBackground->SetPosition(Vector2(-1, 0.5 - Dist / 2.f));
	m.Message->SetPadding(0.1);
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
	Text = new TextRenderer();
	this->Player = Player;

	ScreenOverlay = new UIBackground(UIBox::Orientation::Horizontal, -1, Vector3(1, 0, 0), 2);
	ScreenOverlay->SetOpacity(0);

	CrosshairContainer = new UIBox(UIBox::Orientation::Horizontal, Vector2(-0.05, -0.05 * Graphics::AspectRatio));

	Crosshair = new UIBackground(UIBox::Orientation::Horizontal, 0, Vector3(1), Vector2(0.1, 0.1 * Graphics::AspectRatio));
	Crosshair->SetPadding(0);
	Crosshair->SetUseTexture(true, Texture::LoadTexture("Crosshair"));
	CrosshairContainer->AddChild(Crosshair);

	MoneyNotification = new UIText(0.8, 1, " +15$", Text);
	MoneyNotification->SetPadding(0);
	CrosshairContainer->AddChild(MoneyNotification);
	UIBackground* PlayerStatusBackground = new UIBackground(UIBox::Orientation::Vertical, Vector2(-0.975), 0.075);
	UIBackground* WaveBackground = new UIBackground(UIBox::Orientation::Vertical, Vector2(-0.35, 0.7), 0.075, Vector2(0.7, 0.25));

	PlayerStatusBackground->SetOpacity(0.8);
	WaveBackground->SetOpacity(0.8);

	UIBox* WaveTextBackgrounds[2] =
	{
		new UIBox(UIBox::Orientation::Horizontal, 0),
		new UIBox(UIBox::Orientation::Horizontal, 0)
	};
	WaveBackground->SetBorder(UIBox::BorderType::Rounded, 1);
	PlayerStatusBackground->SetBorder(UIBox::BorderType::Rounded, 1);
	for (auto i : WaveTextBackgrounds)
	{
		i->SetMinSize(Vector2(0.7, 0.1));
		i->SetPadding(0);
		i->SetHorizontalAlign(UIBox::Align::Centered);
		WaveBackground->AddChild(i);
	}


	PlayerHealthText = new UIText(1, 1, "Player: 100%", Text);
	PlayerStatusBackground->AddChild(PlayerHealthText
		->SetPadding(0.01));

	UIBox* PlayerMoneyBar = new UIBox(UIBox::Orientation::Horizontal, 0);
	PlayerMoneyBar->SetPadding(0);

	PlayerMoneyText = new UIText(1, 1, "Money: 0", Text);
	PlayerMoneyBar->AddChild(PlayerMoneyText
		->SetPadding(0.01));
	TransactionText = new UIText(1, Vector3(1, 0.1, 0), "-0", Text);
	TransactionText->IsVisible = false;
	PlayerMoneyBar->AddChild(TransactionText
		->SetPadding(0.01));

	PlayerStatusBackground->AddChild(PlayerMoneyBar);

	TransactionTitle = new UIText(0.8, Vector3(1, 0.1, 0), "Buy ", Text);
	PlayerStatusBackground->AddChild(TransactionTitle
		->SetPadding(0.01));

	WaveText = new UIText(1.25, Vector3(1, 1, 0), "Wave 1: 0%", Text);

	UIBox* TreeBox = new UIBox(UIBox::Orientation::Horizontal, 0);
	TreeBox->SetPadding(0);

	UIBackground* TreeIcon = new UIBackground(UIBox::Orientation::Horizontal, 0, 1, Vector2(0.075));
	TreeIcon->SetSizeMode(UIBox::SizeMode::PixelRelative);
	TreeIcon->SetUseTexture(true, Texture::LoadTexture("TreeIcon"));
	TreeBox->AddChild(TreeIcon);

	TreeHealthText = new UIText(1, 1, "Tree: 100%", Text);
	TreeBox->AddChild(TreeHealthText);

	TreeHealthWarning = new UIBackground(UIBox::Orientation::Horizontal, 0, 1, Vector2(0.075));
	TreeHealthWarning->SetSizeMode(UIBox::SizeMode::PixelRelative);
	TreeHealthWarning->SetUseTexture(true, Texture::LoadTexture("Warning"));
	TreeHealthWarning->SetOpacity(0);
	TreeBox->AddChild(TreeHealthWarning);

	m.MessageBackground = new UIBackground(UIBox::Orientation::Horizontal, Vector2(-1, 0.3), 0.075, Vector2(2, 0));
	m.MessageBackground->SetHorizontalAlign(UIBox::Align::Centered);
	m.MessageBackground->SetOpacity(0);
	m.Message = new UIText(1.75, 1, "", Text);
	m.MessageBackground->AddChild(m.Message);

	WaveTextBackgrounds[1]->AddChild(TreeBox);
	WaveTextBackgrounds[0]->AddChild(WaveText);
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
	ScreenOverlay->SetOpacity((100 - Player->Health) / 150);

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
