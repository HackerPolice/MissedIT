#pragma once

#include "../HeaderFiles.h"

namespace Aimbot
{

#define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))

	inline QAngle RCSLastPunch;

	QAngle ApplyErrorToAngle(QAngle *angles, float margin);

	void NoRecoil(QAngle &angle, CUserCmd *cmd, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon,
	              const bool &silent);

	void AutoCrouch(CUserCmd *cmd, C_BaseCombatWeapon *activeWeapon, const bool &autoCroutch);

	void AutoSlow(C_BasePlayer *localplayer, CUserCmd *cmd, const bool &autoSlow);

	void AutoR8(C_BasePlayer *player, C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd, const bool &autoShoot);

	void AutoShoot(C_BasePlayer *player, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd,
	               Vector &bestspot, QAngle &angle, float &forrwordMove, float &sideMove,
	               const RageWeapon_t &currentSettings);

	void FixMouseDeltas(CUserCmd *cmd, QAngle &angle, QAngle &oldAngle);

	bool canShoot(CUserCmd *cmd, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, Vector &bestSpot,
	              C_BasePlayer *enemy, const RageWeapon_t &currentSettings);

	bool canShoot(C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, const int &hitchance);

	void AutoPistol(C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd, const bool &autoPistol);

	void VelocityExtrapolate(C_BasePlayer *player, Vector &aimPos);
};
