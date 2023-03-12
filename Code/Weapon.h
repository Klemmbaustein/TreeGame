#pragma once
#include <string>
#include <Sound/Sound.h>

struct Weapon
{
	Weapon(float Cooldown, float Damage, float Knockback, std::string Mesh, Sound::SoundBuffer* Sound, float SoundVolume)
	{
		this->Cooldown = Cooldown;
		this->Damage = Damage;
		this->Knockback = Knockback;
		this->Mesh = Mesh;
		this->Sound = Sound;
		this->SoundVolume = SoundVolume;
	}
	float Cooldown = 1;
	float Damage = 15;
	float Knockback = 1;
	float SoundVolume = 1;
	std::string Mesh;
	Sound::SoundBuffer* Sound;
};

struct Pistol : public Weapon
{
	Pistol() : Weapon(0.35, 25, 1, "Pistol", Sound::LoadSound("Pistol"), 0.75)
	{

	}
};

struct SMG : public Weapon
{
	SMG() : Weapon(0.1, 8, 0.25, "SMG", Sound::LoadSound("MP"), 0.5)
	{

	}
};

struct Rifle : public Weapon
{
	Rifle() : Weapon(0.175, 15, 0.5, "Rifle", Sound::LoadSound("Rifle"), 0.5)
	{

	}
};

struct Sniper : public Weapon
{
	Sniper() : Weapon(0.6, 100, 2, "Sniper", Sound::LoadSound("Sniper"), 0.8)
	{

	}
};