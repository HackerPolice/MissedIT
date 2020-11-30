#pragma once

#include "../HeaderFiles.h"

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
    extern int targetAimbot;

	inline LegitWeapon_t* currentWeaponSetting;
	//Hooks
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);

	C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, const LegitWeapon_t& currentSettings);
	C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, C_BasePlayer* localplayer, bool visibleCheck, Vector& bestSpot, float& bestDamage, const LegitWeapon_t& currentSettings);
	void GetClosestSpot(C_BasePlayer* localplayer, C_BasePlayer* enemy, Vector &BestSpot, const LegitWeapon_t& currentSettings);

	void Smooth(C_BasePlayer* player, QAngle& angle, bool& shouldAim, const LegitWeapon_t& currentSettings);
	void Salt(float& smooth, const LegitWeapon_t& currentSettings);
	void AimStep(C_BasePlayer* player, QAngle& angle, CUserCmd* cmd, bool& shouldAim, const LegitWeapon_t& currentSettings);
	bool IsInFov(C_BasePlayer* localplayer, C_BasePlayer* player, const Vector &spot, const LegitWeapon_t& weaponSettings);
	void AutoShoot(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, LegitWeapon_t* currentSettings);
}
