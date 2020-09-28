#include "aimbot.hpp"

void Aimbot::NoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!(cmd->buttons & IN_ATTACK) 
		|| !localplayer
		|| !localplayer->GetAlive())
		return;
	
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if (currentSettings.silent)
	{
		angle.x -= CurrentPunch.x * 2.f;
		angle.y -= CurrentPunch.y * 2.f;
	}
	else if (localplayer->GetShotsFired() > 1 )
	{
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * 2.0;
		angle.y -= NewPunch.y * 2.0;
	}
	RCSLastPunch = CurrentPunch;
}

void Aimbot::AutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    // if (!localplayer || !localplayer->GetAlive() || !Settings::Ragebot::AutoCrouch::enable)
	// 	return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK;
}

void Aimbot::AutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle, const RageWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (currentSettings.autoScopeEnabled && 
		Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && 
		!localplayer->IsScoped() && 
		!(cmd->buttons & IN_ATTACK2) && 
		!(cmd->buttons&IN_ATTACK))
		{ 
			cmd->buttons |= IN_ATTACK2; 
		}
		

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime ) 
		return;

	// QAngle ViewAngle;
	// 	engine->GetViewAngles(ViewAngle);
	// static Vector oldOrigin = localplayer->GetAbsOrigin( );
	// Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin ) * (1.f/globalVars->interval_per_tick);
	// oldOrigin = localplayer->GetAbsOrigin( );
	// float speed  = velocity.Length( );
		
	// if(speed > 15.f)
	// {
	// 	QAngle dir;
	// 	Math::VectorAngles(velocity, dir);
	// 	dir.y = ViewAngle.y - dir.x;
		
	// 	Vector NewMove = Vector(0);
	// 	Math::AngleVectors(dir, NewMove);
	// 	auto max = std::max(forrwordMove, sideMove);
	// 	auto mult = 450.f/max;
	// 	NewMove *= -mult;
			
	// 	forrwordMove = NewMove.x;
	// 	sideMove = NewMove.y;
	// }
	// else 
	// {	
		forrwordMove = 0.f;
		sideMove = 0.f;
	// }
	
	cmd->buttons |= IN_WALK;
}

void Aimbot::AutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
{
    if (!currentSettings.autoShootEnabled)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{		
		cmd->buttons |= IN_ATTACK;

    	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
		if (player)
		{
			// if ( !Ragebot::ragebotPredictionSystem->canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings))
			// {
			// 	AutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
			// 	cmd->buttons &= ~IN_ATTACK;
			// }
		}
		else if (postponeFireReadyTime < globalVars->curtime )
			cmd->buttons &= ~IN_ATTACK;

		return;
	}
}

void Aimbot::AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
{
    if (!currentSettings.autoShootEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return;
		
	// if ( Ragebot::ragebotPredictionSystem->canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings) )
	// {
	// 	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
	// 		cmd->buttons &= ~IN_ATTACK;
	// 	else if ( !(cmd->buttons & IN_ATTACK) )
	// 		cmd->buttons |= IN_ATTACK;
	// 	return;
	// }

	AutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
}

void Aimbot::FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, QAngle& angle, QAngle& oldAngle)
{
    if (!player || !player->GetAlive())
		return;

    QAngle delta = angle - oldAngle;
    const float &sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    const float &m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    const float &m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    const float &zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
    cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}

bool Aimbot::canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon,Vector &bestSpot, C_BasePlayer* enemy,const RageWeapon_t& currentSettings)
{
	if (currentSettings.HitChance == 0)
		return true;
	if (!enemy || !enemy->GetAlive())
		return false;

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

    for (int i = 0; i < 255; i++) {
        static float val1 = (2.0 * M_PI);
	// float b = RandomFloat(0.f, 2.f * M_PI);
    // float spread = weap_spread * RandomFloat(0.f, 1.0f);
    // float d = RandomFloat(0.f, 2.f * M_PI);
    // float inaccuracy = weap_inaccuracy * RandomFloat(0.f, 1.0f);

		double b = RandomeFloat(val1);
        double spread = weap_spread * RandomeFloat(1.0f);
        double d = RandomeFloat(1.0f);
        double inaccuracy = weap_inaccuracy * RandomeFloat(1.0f);

        Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

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

        if (tr.m_pEntityHit == enemy)
           	hitCount++;

        if (static_cast<int>((hitCount/255.f) * 100.f) >= currentSettings.HitChance)
			return true;

		if ((255 - i + hitCount) < NeededHits)
			return false;
    }

    return false;
}


