#include "legitbot.h"


bool Legitbot::aimStepInProgress = false;
std::vector<int64_t> Legitbot::friends = { };
std::vector<long> killTimes = { 0 }; // the Epoch time from when we kill someone

QAngle AimStepLastAngle;
static QAngle RCSLastPunch;

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

/*
* Check Certein Position Is in the Fov Or not
*/
static bool IsInFov(C_BasePlayer* player, const int &BoneID)
{
	if (!player || !player->GetAlive())
		return false;

	using namespace Settings::Legitbot::AutoAim;

	if ( Entity::IsVisibleThroughEnemies(player, BoneID, fov, true) )
		return true;
	
	return false;
}

/* 
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
* It find out he clossest enemy from your View Angle and return the enemy
*/
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
		
		const float &cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
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
/* 
* This Function Return the Clossest Spot from the Crosshair
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
*/
static void GetClosestSpot(C_BasePlayer* localPlayer, C_BasePlayer* enemy, Vector &BestSpot)
{
	if (!localPlayer || !localPlayer->GetAlive())
		return;
	if (!enemy || !enemy->GetAlive())
		return;

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);
	int len = 30;
	if (Settings::Legitbot::minDamage >= 70)
		len = BONE_HEAD;
	else if (Settings::Legitbot::minDamage <= 40)
		len = BONE_RIGHT_ELBOW;
	for( int i = 0; i < len; i++ )
	{
		int boneID = (*modelType).at(i);

		if( boneID == BONE_INVALID )
			continue;

		Vector bone3D = enemy->GetBonePosition(boneID);
		if (!Entity::IsSpotVisibleThroughEnemies(enemy, bone3D, Settings::Legitbot::AutoAim::fov, true))
			return;
		if (!IsInFov(enemy, boneID))
			continue;
		
		AutoWall::FireBulletData data;
		const float &boneDamage = AutoWall::GetDamage(bone3D,localPlayer, true, data);
		if (boneDamage >= 70 && i >= BONE_HEAD)
			break;
		else if (boneDamage <= 40 && i <= BONE_HEAD)
			break;
		else if (boneDamage <= Settings::Legitbot::minDamage) 
			continue;

		if (boneDamage > 0.f)
			BestSpot = bone3D;	
	}
	return;
}

/*
* Send Clossest Player From Crosshair and and the spot to shoot
*/
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

	const bool &IsPriorityBoneInFov = IsInFov( player, (int)(Settings::Legitbot::bone+1)  );
	
	if ( !IsPriorityBoneInFov )
	{
		GetClosestSpot(localplayer, player, eVecTarget);
		if( eVecTarget.IsZero() || !Entity::IsSpotVisibleThroughEnemies(player, eVecTarget) )
			return nullptr;
	}
	
	if (visibleCheck && !Entity::IsSpotVisibleThroughEnemies(player, eVecTarget))
		return nullptr;
	if ( LineGoesThroughSmoke( localplayer->GetEyePosition( ), eVecTarget, true ) )
		return nullptr;

	bestSpot = eVecTarget;

	return player;
}

/*
* Recoil controll System
*/
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

/*
* I don't know What it actuall Does xd
*/
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

	if (!player)
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

static void AutoSlow(C_BasePlayer* player,C_BasePlayer* localplayer, float& forward, float& sideMove, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoSlow::enabled || !player) 
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;

	if ( !activeWeapon || activeWeapon->GetInReload())
		return;

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime )
		return;

	if (Settings::Legitbot::ShootAssist::Hitchance::enabled )
	{
		cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
		return;
	}
    if ( (activeWeapon->GetSpread() + activeWeapon->GetInaccuracy()) == (activeWeapon->GetCSWpnData()->GetMaxPlayerSpeed() / 3.0f) ) 
    {
        cmd->buttons |= IN_WALK;
		forward = 0;
		sideMove = 0;
		cmd->upmove = 0;
    }
}

static void AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove)
{
	// cvar->ConsoleDPrintf(XORSTR("IN AutoShooting \n"));
    if (!Settings::Legitbot::AutoShoot::enabled)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	// cvar->ConsoleDPrintf(XORSTR("AutoShooting \n"));
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return;
		
	if ( Legitbot::canShoot(cmd, localplayer, activeWeapon) )
	{
		// cvar->ConsoleDPrintf(XORSTR(" Going AutoShooting \n"));
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else
			cmd->buttons |= IN_ATTACK;
		return;
	}	
}

/*
 * Add smoothness to the aim but bit more soothness is bad for helth xd
 */
static void Smooth(C_BasePlayer* player, QAngle& angle, bool& shouldAim)
{
	if (!Settings::Legitbot::Smooth::enabled)
		return;
	if (!shouldAim || !player)
		return;
	if (Settings::Legitbot::silent)
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

	if (!player)
		return;

	cmd->buttons |= IN_BULLRUSH | IN_DUCK;
}

static void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
	if (!Settings::Legitbot::AutoPistol::enabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
	if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        cmd->buttons &= ~IN_ATTACK;
}

/*
* Add Fake Mouse movement because vacnet is a bitch
*/
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

bool AimKeyOnly (CUserCmd* cmd)
{
	if (cmd->buttons & IN_ATTACK)
        return true;
    else if (inputSystem->IsButtonDown(Settings::Legitbot::aimkey))
        return true;
    else
        return false;
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

	Legitbot::UpdateValues();

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
			AutoShoot(player, localplayer, activeWeapon, cmd, bestSpot, angle, oldForward, oldSideMove);
			shouldAim = (cmd->buttons&IN_ATTACK);
		}
			
		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

		if (shouldAim)
		{	
			cvar->ConsoleDPrintf(XORSTR("In Should Aim\n"));
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

	}
	else // No player to Shoot
	{
        Settings::Debug::AutoAim::target = Vector(0);	
		lastRandom = QAngle(0);
    }

	AimStep(player, angle, cmd, shouldAim);
	AutoCrouch(player, cmd);
	AutoSlow(player,localplayer, oldForward, oldSideMove, activeWeapon, cmd);
	AutoPistol(activeWeapon, cmd);
	Smooth(player, angle, shouldAim);
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
