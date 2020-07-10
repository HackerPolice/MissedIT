#pragma once

#include "autowall.h"

#include "../../SDK/IInputSystem.h"
#include "../../SDK/IGameEvent.h"
#include "../../SDK/definitions.h"
#include "../../Utils/xorstring.h"
#include "../../Utils/math.h"
#include "../../Utils/entity.h"
#include "../../Utils/bonemaps.h"
#include "../../settings.h"
#include "../../interfaces.h"
#include "../../Hooks/hooks.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
    extern int targetAimbot;
	inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;

	//Hooks
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);
	inline bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon);
	inline void UpdateValues();
}

inline void Legitbot::UpdateValues()
{
	if (!engine->IsInGame() || !Settings::Legitbot::enabled)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer)
		return;
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon)
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();

	const AimbotWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

	Settings::Legitbot::silent = currentWeaponSetting.silent;
	Settings::Legitbot::AutoShoot::enabled = currentWeaponSetting.autoShoot;
	Settings::Legitbot::bone = currentWeaponSetting.bone;
	Settings::Legitbot::aimkey = currentWeaponSetting.aimkey;
	Settings::Legitbot::aimkeyOnly = currentWeaponSetting.aimkeyOnly;
	Settings::Legitbot::Smooth::enabled = currentWeaponSetting.smoothEnabled;
	Settings::Legitbot::Smooth::value = currentWeaponSetting.smoothAmount;
	Settings::Legitbot::Smooth::type = currentWeaponSetting.smoothType;
	Settings::Legitbot::ErrorMargin::enabled = currentWeaponSetting.errorMarginEnabled;
	Settings::Legitbot::ErrorMargin::value = currentWeaponSetting.errorMarginValue;
	Settings::Legitbot::AutoAim::enabled = currentWeaponSetting.autoAimEnabled;
	Settings::Legitbot::AutoAim::fov = currentWeaponSetting.LegitautoAimFov;
	Settings::Legitbot::AimStep::enabled = currentWeaponSetting.aimStepEnabled;
	Settings::Legitbot::AimStep::min = currentWeaponSetting.aimStepMin;
	Settings::Legitbot::AimStep::max = currentWeaponSetting.aimStepMax;
	Settings::Legitbot::AutoPistol::enabled = currentWeaponSetting.autoPistolEnabled;
	Settings::Legitbot::AutoShoot::autoscope = currentWeaponSetting.autoScopeEnabled;
	Settings::Legitbot::RCS::enabled = currentWeaponSetting.rcsEnabled;
	Settings::Legitbot::RCS::always_on = currentWeaponSetting.rcsAlwaysOn;
	Settings::Legitbot::RCS::valueX = currentWeaponSetting.rcsAmountX;
	Settings::Legitbot::RCS::valueY = currentWeaponSetting.rcsAmountY;
	Settings::Legitbot::IgnoreJump::enabled = currentWeaponSetting.ignoreJumpEnabled;
	Settings::Legitbot::IgnoreEnemyJump::enabled = currentWeaponSetting.ignoreEnemyJumpEnabled;
	Settings::Legitbot::Smooth::Salting::enabled = currentWeaponSetting.smoothSaltEnabled;
	Settings::Legitbot::Smooth::Salting::multiplier = currentWeaponSetting.smoothSaltMultiplier;
	Settings::Legitbot::ShootAssist::Hitchance::enabled = currentWeaponSetting.hitchanceEnaled;
	Settings::Legitbot::ShootAssist::Hitchance::value = currentWeaponSetting.hitchance;
	Settings::Legitbot::minDamage = currentWeaponSetting.MinDamage;
	Settings::Legitbot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Triggerbot::enabled = currentWeaponSetting.TriggerBot;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		Settings::Legitbot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];

}

inline bool Legitbot::canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
	if(!localplayer || !localplayer->GetAlive() || !activeWeapon || activeWeapon->GetInReload())
		return false;
	if (!Settings::Legitbot::ShootAssist::Hitchance::enabled)
	{
		if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) >= (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) )
			return true;
		else
			return false;
	}
	
	activeWeapon->UpdateAccuracyPenalty();
	float hitchance = activeWeapon->GetInaccuracy();
	// hitchance = activeWeapon->GetInaccuracy();
	if (hitchance == 0) hitchance = 0.0000001;
	hitchance = 1/(hitchance);
	
	return hitchance >= (Settings::Legitbot::ShootAssist::Hitchance::value*2);
}
