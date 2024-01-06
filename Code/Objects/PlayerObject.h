#pragma once
#include <Objects/WorldObject.h>
#include <GENERATED/PlayerObject.h>
#include <Objects/Components/MoveComponent.h>
#include <Objects/Components/CameraComponent.h>
#include <Objects/Components/MeshComponent.h>
#include <Weapon.h>
#include <Engine/Application.h>

class ParticleComponent;
class GameUI;

class PlayerObject : public WorldObject
{
	MoveComponent* Movement = nullptr;
	CameraComponent* PlayerCamera = nullptr;
	MeshComponent* WeaponMesh = nullptr;
	bool IsEDown = false;
	Application::Timer FootstepTimer;
	Application::Timer PlayerDeathTimer;
	Sound::SoundBuffer* UpgradeSound = nullptr;
	Sound::SoundBuffer* WaterSound = nullptr;
	ParticleComponent* MuzzleFlash = nullptr;
	Vector3 TranslateToWeaponLocation(Vector3 in);
public:
	bool Godmode = false;
	PLAYEROBJECT_GENERATED("Game");
	bool EnemyNearTree = false;
	void OnMoneyAdded(uint64_t Amount);
	uint64_t RecentlyAddedMoney = 0;
	float MoneyTimer = 0;
	static PlayerObject* GetPlayer();
	uint64_t CurrentWave = 0;
	float WaveTimer = 0;
	bool WaveActive = false;
	uint64_t TransactionMoney = 0;
	std::string TransactionString;
	Sound::SoundBuffer* HitSound = nullptr;
	Sound::SoundBuffer* Footsteps[4] = {nullptr, nullptr, nullptr, nullptr};
	float ShootCooldown = 0;
	float Health = 100.f;
	uint64_t Money = 0;
	bool RecentlyHit = false;
	GameUI* DisplayedUI = nullptr;
	Weapon CurrentWeapon = Pistol();

	void DealDamage(float Amount);
	void SetWeapon(Weapon NewWeapon);
	void Begin() override;
	void Update() override;
	void Destroy() override;

	// Quits to main menu and saves high score if needed.
	void QuitGameToMenu();
};