#include "legitbot.h"


bool Legitbot::aimStepInProgress = false;
std::vector<int64_t> Legitbot::friends = { };
std::vector<long> killTimes = { 0 }; // the Epoch time from when we kill someone

QAngle AimStepLastAngle;
QAngle RCSLastPunch;

int Legitbot::targetAimbot = -1;

static QAngle ApplyErrorToAngle(QAngle* angles, float margin)
{
	QAngle error;
	error.Random(-1.0f, 1.0f);
	error *= margin;
	angles->operator+=(error);
	return error;
}

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos, Vector& localeye)
{
	if (!player || !player->GetAlive())
		return;
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
	localeye += (player->GetVelocity() * globalVars->interval_per_tick);
}

static bool IsInFov(C_BasePlayer* player, const int &BoneID)
{
	if (!player || !player->GetAlive())
		return false;
	using namespace Settings::Legitbot::AutoAim;
	if ( Entity::IsVisibleThroughEnemies(player, BoneID, true) && Entity::IsSpotVisible(player, BoneID, true) )
		return true;
	
	return false;
}

static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;

	float bestFov = Settings::Legitbot::AutoAim::fov;

	Vector pVecTarget = localplayer->GetEyePosition();
	C_BasePlayer* enemy = nullptr;
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	for (int i = engine->GetMaxClients(); i > 1 ; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
	    	|| player == localplayer
	    	|| player->GetDormant()
	    	|| !player->GetAlive()
	    	|| player->GetImmune())
	    	continue;

		if (Entity::IsTeamMate(player, localplayer))
	   	 	continue;

		Vector cbVecTarget = player->GetEyePosition();
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		if (cbFov > bestFov)
			continue;
		if (cbFov < bestFov)
		{
			bestFov = cbFov;
			enemy = player;
		}
	}

	return enemy;
}

static void GetClosestSpot(C_BasePlayer* localPlayer, C_BasePlayer* enemy, Vector &BestSpot)
{
	if (!localPlayer || !localPlayer->GetAlive())
		return;
	if (!enemy || !enemy->GetAlive())
		return;

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

	float bestFov = Settings::Legitbot::AutoAim::fov;

	int len = 30;
	if (Settings::Legitbot::MinDamage::enabled && Settings::Legitbot::MinDamage::value >= 70)
		len = BONE_HEAD;
	else if (Settings::Legitbot::MinDamage::enabled && Settings::Legitbot::MinDamage::value >= 70)
		len = BONE_RIGHT_ELBOW;

	for( int i = 0; i < len; i++ )
	{
		int boneID = (*modelType).at(i);

		if( boneID == BONE_INVALID )
			continue;

		Vector bone3D = enemy->GetBonePosition(boneID);

		if (!IsInFov(enemy, boneID))
			continue;
		if (!Settings::Legitbot::MinDamage::enabled)
		{
			Vector pVecTarget = localPlayer->GetEyePosition();
			QAngle viewAngles;
				engine->GetViewAngles(viewAngles);

			Vector cbVecTarget = enemy->GetEyePosition();
			float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
			
			if (cbFov < 3.f)
			{
				BestSpot = BestSpot;
				return;
			}
			if (cbFov < bestFov)
			{
				bestFov = cbFov;
				BestSpot = BestSpot;
			}
			continue;
		}
		
		int boneDamage = AutoWall::GetDamage(bone3D, true);

		if (boneDamage >= 70 && i >= BONE_HEAD)
			break;
		else if (boneDamage <= 40 && i <= BONE_HEAD)
			break;
		else if (boneDamage <= Settings::Legitbot::MinDamage::value) 
			continue;

		if (boneDamage > 0.f)
			BestSpot = bone3D;	
	}
}

static void RCS(QAngle& angle, C_BasePlayer* player,C_BasePlayer* localplayer, CUserCmd* cmd, bool& shouldAim)
{
	if ( !localplayer->GetAlive() || !localplayer)
		return;

	if (!Settings::Legitbot::RCS::enabled)
		return;

	if (!(cmd->buttons & IN_ATTACK))
		return;

	if (!Settings::Legitbot::RCS::always_on && !shouldAim)
		return;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if ( Settings::Legitbot::silent)
	{
		angle.x -= CurrentPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= CurrentPunch.y * Settings::Legitbot::RCS::valueY;
		return;
	}
	
	if (localplayer->GetShotsFired() > 1 )
	{
		cvar->ConsoleDPrintf("RCS ON\n");
		QAngle NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * Settings::Legitbot::RCS::valueX;
		angle.y -= NewPunch.y * Settings::Legitbot::RCS::valueY;
	}
	RCSLastPunch = CurrentPunch;
}

static void AimStep(C_BasePlayer* player, QAngle& angle, CUserCmd* cmd, bool& shouldAim)
{
	if (!Settings::Legitbot::AimStep::enabled)
		return;

	if (!Settings::Legitbot::AutoAim::enabled)
		return;

	if (Settings::Legitbot::Smooth::enabled)
		return;

	if (!shouldAim)
		return;

	if (!Legitbot::aimStepInProgress)
		AimStepLastAngle = cmd->viewangles;

	if (!player || !player->GetAlive())
		return;

	float fov = Math::GetFov(AimStepLastAngle, angle);

	Legitbot::aimStepInProgress = ( fov > (Math::float_rand(Settings::Legitbot::AimStep::min, Settings::Legitbot::AimStep::max)) );

	if (!Legitbot::aimStepInProgress)
		return;

    cmd->buttons &= ~(IN_ATTACK); // aimstep in progress, don't shoot.

	QAngle deltaAngle = AimStepLastAngle - angle;

	Math::NormalizeAngles(deltaAngle);
	float randX = Math::float_rand(Settings::Legitbot::AimStep::min, std::min(Settings::Legitbot::AimStep::max, fov));
	float randY = Math::float_rand(Settings::Legitbot::AimStep::min, std::min(Settings::Legitbot::AimStep::max, fov));
	if (deltaAngle.y < 0)
		AimStepLastAngle.y += randY;
	else
		AimStepLastAngle.y -= randY;

	if(deltaAngle.x < 0)
		AimStepLastAngle.x += randX;
	else
		AimStepLastAngle.x -= randX;

	angle = AimStepLastAngle;
}

static void Salt(float& smooth)
{
	float sine = sin (globalVars->tickcount);
	float salt = sine * Settings::Legitbot::Smooth::Salting::multiplier;
	float oval = smooth + salt;
	smooth *= oval;
}

static bool AutoSlow(C_BasePlayer* player,C_BasePlayer* localplayer, float& forward, float& sideMove, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoSlow::enabled )
		return true;
	if (!player || !player->GetAlive()) 
		return false;
	if (!localplayer || !localplayer->GetAlive())
		return false;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return false;
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime )
		return false;

	if (Settings::Legitbot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK) )
	{
		cmd->buttons |= IN_ATTACK2;
		return false;
	}

	if (Settings::Legitbot::Hitchance::enabled )
	{
		cmd->buttons |= IN_WALK;
		forward = -forward;
		sideMove = -sideMove;
		cmd->upmove = 0;
		return false;
	}
    if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) == (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) ) 
    {
        cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
    }
	
	return false;
}

static bool canShoot(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
	if(!localplayer || !localplayer->GetAlive() )
		return false;
	if (!activeWeapon || activeWeapon->GetInReload())
		return false;
	if (!Settings::Legitbot::Hitchance::enabled)
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
	
	return hitchance >= (Settings::Legitbot::Hitchance::value*2);
}

static bool AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove)
{
    if (!Settings::Legitbot::AutoShoot::enabled)
		return true;
	if (!localplayer || !localplayer->GetAlive())
		return false;
	if (!player || !player->GetAlive())
		return false;
	if (!activeWeapon || activeWeapon->GetInReload())
		return false;
	
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return true;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return true;
		
	if ( canShoot(localplayer, activeWeapon) )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;
		
		return true;
	}	

	return AutoSlow(player,localplayer, forrwordMove, sideMove, activeWeapon, cmd);
}

static void Smooth(C_BasePlayer* player, QAngle& angle, bool& shouldAim)
{
	if (!Settings::Legitbot::Smooth::enabled)
		return;
	if (!shouldAim || !player || !player->GetAlive())
		return;

	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);

	QAngle delta = angle - viewAngles;
	Math::NormalizeAngles(delta);

	float smooth = powf(Settings::Legitbot::Smooth::value, 0.4f); // Makes more slider space for actual useful values

	smooth = std::min(0.99f, smooth);

	if (Settings::Legitbot::Smooth::Salting::enabled)
		Salt(smooth);

	QAngle toChange = {0,0,0};

	SmoothType type = Settings::Legitbot::Smooth::type;

	if (type == SmoothType::SLOW_END)
		toChange = delta - (delta * smooth);
	else if (type == SmoothType::CONSTANT || type == SmoothType::FAST_END)
	{
		float coeff = (1.0f - smooth) / delta.Length() * 4.f;

		if (type == SmoothType::FAST_END)
			coeff = powf(coeff, 2.f) * 10.f;

		coeff = std::min(1.f, coeff);
		toChange = delta * coeff;
	}

	angle = viewAngles + toChange;
}

static void AutoCrouch(C_BasePlayer* player, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoCrouch::enabled)
		return;

	if (!player || !player->GetAlive())
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

static void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!activeWeapon || activeWeapon->GetInReload())
	if (!Settings::Legitbot::AutoPistol::enabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
	if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        cmd->buttons &= ~IN_ATTACK;
}

static void FixMouseDeltas(CUserCmd* cmd, const QAngle &angle, const QAngle &oldAngle, bool &shouldAim)
{
    if( !shouldAim)
        return;
    QAngle delta = angle - oldAngle;
    float sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    float m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    float m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    float zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / ( m_yaw * sens * zoomMultiplier );
    cmd->mousedy = delta.x / ( m_pitch * sens * zoomMultiplier );
}

static bool AimKeyOnly (CUserCmd* cmd)
{
	if (cmd->buttons & IN_ATTACK)
        return true;
    else if (inputSystem->IsButtonDown(Settings::Legitbot::aimkey))
        return true;
    else
        return false;
}

static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd, C_BasePlayer* localplayer, bool visibleCheck, Vector& bestSpot, float& bestDamage)
{
	if (!localplayer || !localplayer->GetAlive() )
		return nullptr;

	C_BasePlayer *player = GetClosestEnemy(localplayer); // getting the closest enemy to the crosshair

	if ( !player || !player->GetAlive())
		return nullptr;

	if ( localplayer->IsFlashed() )
		return nullptr;
	
	Vector eVecTarget = player->GetBonePosition( (int)(Settings::Legitbot::bone+1) );

	bool IsPriorityBoneInFov = IsInFov( player, (int)(Settings::Legitbot::bone+1)  );
	
	if ( !IsPriorityBoneInFov )
	{
		GetClosestSpot(localplayer, player, eVecTarget);
		if( eVecTarget.IsZero())
			return nullptr;
	}
	
	if ( LineGoesThroughSmoke( localplayer->GetEyePosition( ), eVecTarget, true ) )
		return nullptr;

	bestSpot = eVecTarget;

	return player;
}

static void UpdateValues()
{
	if (!engine->IsInGame())
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer || !localplayer->GetAlive())
		return;
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeWeapon->GetItemDefinitionIndex();

	const LegitWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);

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
	Settings::Legitbot::Hitchance::enabled = currentWeaponSetting.hitchanceEnaled;
	Settings::Legitbot::Hitchance::value = currentWeaponSetting.hitchance;
	Settings::Legitbot::MinDamage::enabled = currentWeaponSetting.mindamage;
	Settings::Legitbot::MinDamage::value = currentWeaponSetting.minDamagevalue;
	Settings::Legitbot::AutoSlow::enabled = currentWeaponSetting.autoSlow;
	Settings::Triggerbot::enabled = currentWeaponSetting.TriggerBot;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		Settings::Legitbot::AutoAim::desiredBones[bone] = currentWeaponSetting.desiredBones[bone];

}

void Legitbot::CreateMove(CUserCmd* cmd)
{
	if(!Settings::Legitbot::enabled)
		return;
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;
	C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload())
		return;

	UpdateValues();

	QAngle oldAngle;
		engine->GetViewAngles(oldAngle);
	
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;
	bool shouldAim = false;
	QAngle angle = cmd->viewangles;
	static QAngle lastRandom = QAngle(0);
	
	Vector localEye = localplayer->GetEyePosition();

	if (Settings::Legitbot::IgnoreJump::enabled && (!(localplayer->GetFlags() & FL_ONGROUND) && localplayer->GetMoveType() != MOVETYPE_LADDER))
		return;

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

    Vector bestSpot = Vector(0);
	float bestDamage = float(0);

	C_BasePlayer* player = GetClosestPlayerAndSpot(cmd, localplayer, true, bestSpot, bestDamage);

	if (player)
	{
		if (Settings::Legitbot::aimkeyOnly)
			shouldAim = AimKeyOnly(cmd); 
		else 
		{
			if ( !AutoShoot(player, localplayer, activeWeapon, cmd, bestSpot, angle, oldForward, oldSideMove) )
				return;
			shouldAim = (cmd->buttons&IN_ATTACK);
		}
			
		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

		if (shouldAim)
		{	
			if (Settings::Legitbot::Prediction::enabled)
				VelocityExtrapolate(player, bestSpot, localEye); // get eye pos next tick

			angle = Math::CalcAngle(localEye, bestSpot);

			if (Settings::Legitbot::ErrorMargin::enabled)
			{
				static int lastShotFired = 0;
				if ((localplayer->GetShotsFired() > lastShotFired)) //get new random spot when firing a shot or when aiming at a new target
					lastRandom = ApplyErrorToAngle(&angle, Settings::Legitbot::ErrorMargin::value);

				angle += lastRandom;
				lastShotFired = localplayer->GetShotsFired();
			}
		}

		AimStep(player, angle, cmd, shouldAim);
		AutoCrouch(player, cmd);
		AutoPistol(activeWeapon, cmd);
		Smooth(player, angle, shouldAim);

	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = Vector(0);	
		lastRandom = QAngle(0);
    }

	RCS(angle, player,localplayer, cmd, shouldAim);
	
    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

	FixMouseDeltas(cmd, angle, oldAngle, shouldAim);
	
	cmd->viewangles = angle;

	if( !Settings::Legitbot::silent )
    	engine->SetViewAngles(angle);

	Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);

}

void Legitbot::FireGameEvent(IGameEvent* event)
{
	if(!Settings::Legitbot::enabled || !event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0 )
	{
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
			return;
		Legitbot::friends.clear();
	}
	if( strcmp(event->GetName(), XORSTR("player_death")) == 0 )
	{
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
			return;

		if (attacker_id != engine->GetLocalPlayer())
			return;

		killTimes.push_back(Util::GetEpochTime());
	}
}
