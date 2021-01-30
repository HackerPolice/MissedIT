#pragma once

#include "../HeaderFiles.h"

namespace Legitbot
{
	extern std::vector<int64_t> friends;
	extern int targetAimbot;

	inline LegitWeapon_t *currentWeaponSetting;
	inline matrix3x4_t BoneMatrix[128];
	inline C_BasePlayer *localplayer = nullptr;
	inline C_BaseCombatWeapon *activeWeapon = nullptr;

	//Hooks
	void CreateMove(CUserCmd *cmd);

	void FireGameEvent(IGameEvent *event);

}
