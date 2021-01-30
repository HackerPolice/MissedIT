#include "aimbot.hpp"

QAngle Aimbot::ApplyErrorToAngle(QAngle *angles, float margin)
{
	QAngle error;
	error.Random(-1.0f, 1.0f);
	error *= margin;
	angles->operator+=(error);
	return error;
}

void Aimbot::NoRecoil(QAngle &angle, CUserCmd *cmd, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon,
                      const bool &silent)
{
	if (!(cmd->buttons & IN_ATTACK)) {
		return;
	}

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 ||
	    *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP) {
		return;
	}

	static QAngle *CurrentPunch;
	CurrentPunch = localplayer->GetAimPunchAngle();

	if (silent) {
		angle.x -= CurrentPunch->x * 2.f;
		angle.y -= CurrentPunch->y * 2.f;
	} else if (localplayer->GetShotsFired() > 1) {
		QAngle NewPunch = {CurrentPunch->x - RCSLastPunch.x, CurrentPunch->y - RCSLastPunch.y, 0};

		angle.x -= NewPunch.x * 2.f;
		angle.y -= NewPunch.y * 2.f;
	}
	RCSLastPunch = *CurrentPunch;
}

void Aimbot::AutoCrouch(CUserCmd *cmd, C_BaseCombatWeapon *activeWeapon, const bool &autoCrouch)
{
	if (!autoCrouch) {
		return;
	} else if (!(cmd->buttons & IN_ATTACK)) {
		return;
	}

	cmd->buttons |= IN_DUCK | IN_BULLRUSH;
}

void Aimbot::AutoSlow(C_BasePlayer *localplayer, CUserCmd *cmd, const bool &autoSlow)
{
	if (!autoSlow) { return; }

	static float speed = 0;

	float length = localplayer->GetVelocity().Length();

	cmd->forwardmove = 0;
	cmd->sidemove = 0;

	speed = length;
}

void Aimbot::AutoR8(C_BasePlayer *player, C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd, const bool &autoShoot)
{
	if (!autoShoot) {
		return;
	} else if (!activeWeapon || activeWeapon->GetInReload()) {
		return;
	} else if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER) {
		return;
	} else if (activeWeapon->GetAmmo() == 0) {
		return;
	}

	cmd->buttons |= IN_ATTACK;
	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
	if (postponeFireReadyTime > 0) {
		if (postponeFireReadyTime < globalVars->curtime) {
			if (player) {
				return;
			}
			cmd->buttons &= ~IN_ATTACK;
		}
	}
}

void Aimbot::AutoShoot(C_BasePlayer *player, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd,
                       Vector &bestspot, QAngle &angle, float &forrwordMove, float &sideMove,
                       const RageWeapon_t &currentSettings)
{
	if (!currentSettings.autoShootEnabled) {
		return;
	}
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0) {
		return;
	}
	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 ||
	    weaponType == CSWeaponType::WEAPONTYPE_GRENADE) {
		return;
	}
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER) {
		return;
	}

	// if ( Ragebot::ragebotPredictionSystem->canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings) )
	// {
	// 	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
	// 		cmd->buttons &= ~IN_ATTACK;
	// 	else if ( !(cmd->buttons & IN_ATTACK) )
	// 		cmd->buttons |= IN_ATTACK;
	// 	return;
	// }

	AutoSlow(localplayer, cmd, true);
}

void Aimbot::FixMouseDeltas(CUserCmd *cmd, QAngle &angle, QAngle &oldAngle)
{
	QAngle delta = angle - oldAngle;
	const float &sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
	const float &m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
	const float &m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
	const float &zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

	Math::NormalizeAngles(delta);

	cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
	cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}

bool Aimbot::canShoot(CUserCmd *cmd, C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, Vector &bestSpot,
                      C_BasePlayer *enemy, const RageWeapon_t &currentSettings)
{
	if (currentSettings.HitChance == 0) {
		return true;
	}
	if (!enemy || !enemy->IsAlive()) {
		return false;
	}

	Vector src = localplayer->GetEyePosition();
	QAngle angle = Math::CalcAngle(src, bestSpot);
	Math::NormalizeAngles(angle);

	Vector forward, right, up;
	Math::AngleVectors(angle, &forward, &right, &up);

	int hitCount = 0;
	int NeededHits = static_cast<int>(255.f * (currentSettings.HitChance / 100.f));

	activeWeapon->UpdateAccuracyPenalty();
	float weap_spread = activeWeapon->GetSpread();
	float weap_inaccuracy = activeWeapon->GetInaccuracy();

	static float val1 = (2.0 * M_PI);

	for (int i = 0; i < 255; i++) {

		double b = RandomeFloat(val1);
		double spread = weap_spread * RandomeFloat(1.0f);
		double d = RandomeFloat(1.0f);
		double inaccuracy = weap_inaccuracy * RandomeFloat(1.0f);

		Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread),
		                  0), direction;

		direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalize();

		QAngle viewAnglesSpread;
		Math::VectorAngles(direction, up, viewAnglesSpread);
		Math::NormalizeAngles(viewAnglesSpread);

		Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * activeWeapon->GetCSWpnData()->GetRange());

		trace_t tr;
		Ray_t ray;

		ray.Init(src, viewForward);
		trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, enemy, &tr);

		if (tr.m_pEntityHit == enemy) {
			hitCount++;
		}

		if (static_cast<int>((hitCount / 255.f) * 100.f) >= currentSettings.HitChance) {
			return true;
		}

		if ((255 - i + hitCount) < NeededHits) {
			return false;
		}
	}

	return false;
}

bool Aimbot::canShoot(C_BasePlayer *localplayer, C_BaseCombatWeapon *activeWeapon, const int &ReqHitchance)
{
	if (!localplayer || !localplayer->IsAlive()) {
		return false;
	}
	if (!activeWeapon || activeWeapon->GetInReload()) {
		return false;
	}
	if (ReqHitchance == 0) {
		if ((activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) <=
		    (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f)) {
			return true;
		} else {
			return false;
		}
	}

	activeWeapon->UpdateAccuracyPenalty();
	float hitchance = activeWeapon->GetInaccuracy();
	// hitchance = activeWeapon->GetInaccuracy();
	if (hitchance == 0) { hitchance = 0.0000001; }
	hitchance = 1 / (hitchance);

	return hitchance >= (ReqHitchance * 2);
}

void Aimbot::VelocityExtrapolate(C_BasePlayer *player, Vector &aimPos)
{
	if (!player || !player->IsAlive()) {
		return;
	}
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
}

void Aimbot::AutoPistol(C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd, const bool &autoPistol)
{
	if (!autoPistol) {
		return;
	} else if (!activeWeapon || activeWeapon->GetInReload()) {
		return;
	} else if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime) {
		return;
	} else if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL) {
		return;
	} else if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER) {
		cmd->buttons &= ~IN_ATTACK;
	}
}

