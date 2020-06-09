#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.h"
#include "autowall.h"

#include "../Utils/bonemaps.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "backtrack.h"

#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>

#define PI_F (3.14)
#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define TICK_INTERVAL			(globalVars->interval_per_tick)


#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#ifndef NormalizeNo
	#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )
#endif

#ifndef GetPercentVal
    #define GetPercentVal(val, percent) ( val * (percent/100) )
#endif

#ifndef GetPercent
	#define GetPercent(total, val) ( (val/total) * 100)
#endif

std::vector<int64_t> Ragebot::friends = {};
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone

inline bool	EnemyPresent = false,
			doubleFire = false,
			DieBitch = false;

inline int DeathBoneIndex = 0, MaxEnemyRender = 2;
const int MultiVectors = 7;
static Vector DoubleTapSpot = Vector(0);

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos)
{
	// cvar->ConsoleDPrintf(XORSTR("Lenth : %f\n"), player->GetVelocity().Length2D());
	// if (player->GetVelocity().Length2D() < 125.f)
	// 	return aimPos;
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
}

/* Fills points Vector. True if successful. False if not.  Credits for Original method - ReactiioN */
static bool HeadMultiPoint(C_BasePlayer* player, Vector* points[], matrix3x4_t boneMatrix[])
{

	model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)CONST_BONE_HEAD, 0);
    
	if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    // Vector center = player->GetBonePosition(CONST_BONE_HEAD);

	// 0 - center, 1 - forehead, 2 - skullcap,
	// 3 - leftear, 4 - rightear, 5 - nose, 6 - backofhead

	// *points[MultiVectors] = (center,center,center,center,center,center);
	Vector center = ( mins + maxs ) * 0.5f;
	 // OLD CODE
    for (int i = 0; i < MultiVectors; i++) // set all points initially to center mass of head.
		*points[i] = center;
	Util::Log(XORSTR("Center Created"));
    points[1]->z += bbox->radius * 0.60f; // morph each point.
	points[2]->z += bbox->radius * 1.35f; // ...
	points[3]->y -= bbox->radius * 0.65f;
	points[3]->z += bbox->radius * 0.90f;
	points[4]->x += bbox->radius * 0.80f;
	points[5]->x -= bbox->radius * 0.80f;
	points[6]->y += bbox->radius * 0.80f;
	points[7]->y -= bbox->radius * 0.80f;

    return true;
}

static bool UpperChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{

	model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* upChst = modelInfo->GetStudioModel(pModel);
    if (!upChst)
		return false;
    
	mstudiobbox_t* bbox = upChst->pHitbox((int)CONST_BONE_UPPER_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;

	points[MultiVectors] = (center, center, center, center, center, center,center);
    
    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}
 
static bool ChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{

    model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)CONST_BONE_MIDDLE_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;
	/*
	* To redunce time we directly implement the values rather than ... using for loop
	*/
	points[MultiVectors] = (center,center,center,center,center,center,center);

    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}

static bool LowerChestMultiPoint(C_BasePlayer* player, Vector points[], matrix3x4_t boneMatrix[])
{
    model_t* pModel = player->GetModel();
    if (!pModel)
		return false;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return false;
    
	mstudiobbox_t* bbox = hdr->pHitbox((int)CONST_BONE_LOWER_SPINE, 0);
    if (!bbox)
		return false;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

    Vector center = (mins + maxs) * 0.5f;

	/*
	* To redunce time we directly implement the values rather than ... using for loop
	*/
	points[MultiVectors] = (center,center,center,center,center,center,center);
    
    points[1].y -= bbox->radius * 0.80f;
    points[1].z += bbox->radius * 0.90f;
    points[2].z += bbox->radius * 1.25f;
    points[3].y += bbox->radius * 0.80f;
    points[4].x += bbox->radius * 0.80f;
    points[5].x -= bbox->radius * 0.80f;
    points[6].y -= bbox->radius * 0.80f;

    return true;
}


/*
** Method for safety damage prediction where 
** It will just look for required Damage Not for the best damage
*/
static void GetDamageAndSpots(C_BasePlayer* player, Vector &spot, float& damage, float& playerHelth, int& i)
{
	using namespace Settings;

    const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);	

	damage = 0.f;
	spot = Vector(0);

	matrix3x4_t boneMatrix[128];

	if ( !player->SetupBones(boneMatrix, 128, 0x100, 0) )
		return;

	if (!Settings::Ragebot::AutoAim::desiredBones[i])
	    return;

	int boneID = (*modelType).at(i);

	if (boneID == BONE_INVALID) // bone not available on this modeltype.
	   	return;
	else 
	{
		cvar->ConsoleDPrintf(XORSTR("BONE ID : %d"), boneID);
		Vector bone3D = player->GetBonePosition(boneID);
		VelocityExtrapolate(player, bone3D);
		
		Autowall::FireBulletData data;
		float spotDamage = Autowall::GetDamage(bone3D, true, data);
		cvar->ConsoleDPrintf(XORSTR("\tDamage : %f \n"), spotDamage);
		if (spotDamage >= playerHelth)
		{
			damage = spotDamage;
		    spot = bone3D;
			DieBitch = true;
			return;
		}
		damage = spotDamage;
		spot = bone3D;
    }
	
}


/*
** Get best Damage from the enemy and the spot
*/
static void GetBestSpotAndDamage(C_BasePlayer* player, Vector& Spot, float& Damage)
{
	
	int len = sizeof(Settings::Ragebot::AutoAim::desiredBones) / sizeof(Settings::Ragebot::AutoAim::desiredBones[0]);

	Vector spot;
	float damage;

	float playerHelth = player->GetHealth();

		
	// For safety mesurements
	if (Settings::Ragebot::damagePrediction == DamagePrediction::justDamage)
	{	
		for (int i = 0; i < len; i++)
		{	
			GetDamageAndSpots(player, spot, damage, playerHelth, i);
			if (DieBitch)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			else if (damage > Settings::Ragebot::AutoWall::value)
			{
				Spot = spot;
				Damage = damage;
				return;
			}

		}
		
	}
	
	else if (Settings::Ragebot::damagePrediction == DamagePrediction::damage)
	{
		float prevdamage = 0;
		for (int i = 0; i < len; i++)
		{	
			GetDamageAndSpots(player, spot, damage, playerHelth, i);
			if (DieBitch)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			if (damage > prevdamage)
			{
				Spot = spot;
				prevdamage = Damage = damage;
			}
		}
	}

}

/*
* To find the closesnt enemy to reduce the calculation time and increase performace
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
*/
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer, CUserCmd* cmd)
{
	C_BasePlayer* closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	for (int i = engine->GetMaxClients(); i > 1; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*)entityList->GetClientEntity(i);

		if (!player
	    	|| player == localplayer
	    	|| player->GetDormant()
	    	|| !player->GetAlive()
	    	|| player->GetImmune())
	    	continue;

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
	   	 	continue;

		Vector cbVecTarget = player->GetEyePosition();
		if (Entity::IsSpotVisible(player, cbVecTarget));
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
	
		if ( cbFov < prevFOV || prevFOV == 0.f)
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
	}
	return closestPlayer;
}

// get the clossest player from crosshair
static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& bestSpot, float& bestDamage)
{
	if (!localplayer->GetAlive())
		return nullptr;

	C_BasePlayer* player = GetClosestEnemy(localplayer, cmd);
	
	if ( !player )
		return nullptr;
			
	GetBestSpotAndDamage(player, bestSpot, bestDamage);
	
	bool Visible = Entity::IsSpotVisible(player, bestSpot);
	if ( Visible && bestDamage >= Settings::Ragebot::visibleDamage)
		return player;
	else if (!Visible && bestDamage >= Settings::Ragebot::AutoWall::value)
		return player;
	else 
		return nullptr;
}

// Get the best damage and the player 
static C_BasePlayer* GetBestEnemyAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& BestSpot, float& BestDamage)
{

    C_BasePlayer* closestEntity = nullptr;

	if (!localplayer->GetAlive())
		return nullptr;

	int maxClietn = engine->GetMaxClients();
	Vector bestSpot = Vector(0);
	float bestDamage = 0.f;
	float prevDamage = 0.f;

	for (int i = engine->GetMaxClients(); i  > 1; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || player == localplayer || player->GetDormant() || !player->GetAlive() || player->GetImmune())
			continue;

		if (Entity::IsTeamMate(player, localplayer)) // Checking for Friend If any it will continue to next player
			continue;

		// auto temp = std::async(std::launch::async, GetBestSpotAndDamage, player, &bestSpot[i], &bestDamage[i]);
		GetBestSpotAndDamage(player, bestSpot, bestDamage);

		if (bestDamage >= player->GetHealth())
		{
			DieBitch = false;
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return player;
		}
		else if (bestDamage > BestDamage)
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			closestEntity = player;
		}
	}
	bool Visible = Entity::IsSpotVisible(closestEntity, BestSpot, 180.f);
	if ( Visible && BestDamage > Settings::Ragebot::visibleDamage)
		return closestEntity;
	else if (!Visible && BestDamage > Settings::Ragebot::AutoWall::value)
		return closestEntity;
	else 
		return nullptr;
}

//Hitchance source from nanoscence
static float Ragebothitchance(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
	float hitchance = 10;
	activeWeapon->UpdateAccuracyPenalty();
	if (activeWeapon)
	{
		float inaccuracy = activeWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		hitchance = 1 / inaccuracy;
		
		return hitchance;
	}
	return hitchance;
}

static void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
    if (!(cmd->buttons & IN_ATTACK))
		return;

	float aimpunch = cvar->FindVar("weapon_recoil_scale")->GetFloat();
	if (!aimpunch || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return;
	
	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();
	angle.x -= CurrentPunch.x * 2.f;
	angle.y -= CurrentPunch.y * 2.f;

}

// AutoCroutch is a bad idea in hvh instant death if you miss
static void RagebotAutoCrouch(C_BasePlayer* player, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon)
{
    if (!player || !Settings::Ragebot::AutoCrouch::enable)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK;
}


static void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove)
{
	if (!Settings::Ragebot::AutoSlow::enabled)
		return;
	
	if (!localplayer)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime || !activeWeapon || activeWeapon->GetAmmo() == 0)
		return;
	
	float hc = Ragebothitchance(localplayer,activeWeapon);
	if (Settings::Ragebot::HitChanceOverwrride::enable)
		hc -= GetPercentVal(hc, Settings::Ragebot::HitChanceOverwrride::value);

	if (hc <= (Settings::Ragebot::HitChance::value * 1.5))
	{
		float curTime = globalVars->curtime;
		QAngle ViewAngle = cmd->viewangles;
		Math::ClampAngles(ViewAngle);
		static auto oldorigin = localplayer->GetAbsOrigin();
		Vector unpredictedVal = ( localplayer->GetAbsOrigin() - oldorigin ) * ( 1.0 / globalVars->interval_per_tick );
		Vector velocity = localplayer->GetVelocity();
		float speed  = velocity.Length();
		
		if(speed > 15.f)
		{
			QAngle dir;
			Math::VectorAngles(velocity, dir);
			dir.y = cmd->viewangles.y - dir.x;

			Vector NewMove = Vector(0);
			Math::AngleVectors(dir, NewMove);
			float max = std::max( std::fabs( forrwordMove ), std::fabs( sideMove ) );
			float mult = 450.f / max;
			
			forrwordMove = NewMove.x;
			sideMove = NewMove.y;
		} 
	}
}

static void RagebotAutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd)
{
    if (!Settings::Ragebot::AutoPistol::enabled)
    	return;

    if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;

    if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
		cmd->buttons &= ~IN_ATTACK;
}

static void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot)
{
    if (!Settings::Ragebot::AutoShoot::enabled)
		return;

	if (!activeWeapon || activeWeapon->GetAmmo() == 0)
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;

	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
	{		
		cmd->buttons |= IN_ATTACK;

    	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
		if (postponeFireReadyTime > 0 && postponeFireReadyTime < globalVars->curtime && !player)
			cmd->buttons &= ~IN_ATTACK;

		return;
	}

	float hc = Ragebothitchance(localplayer, activeWeapon);
	
	if (Settings::Ragebot::HitChanceOverwrride::enable)
		hc -= GetPercentVal(hc, Settings::Ragebot::HitChanceOverwrride::value);
	
	if ( hc <= Settings::Ragebot::HitChance::value * 1.5)
		return;	

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
	    cmd->buttons &= ~IN_ATTACK;
	else
		cmd->buttons |= IN_ATTACK;
}

static void FixMouseDeltas(CUserCmd* cmd,C_BasePlayer* player, const QAngle& angle, const QAngle& oldAngle)
{
    if (!player)
		return;

    QAngle delta = angle - oldAngle;
    float sens = cvar->FindVar(XORSTR("sensitivity"))->GetFloat();
    float m_pitch = cvar->FindVar(XORSTR("m_pitch"))->GetFloat();
    float m_yaw = cvar->FindVar(XORSTR("m_yaw"))->GetFloat();
    float zoomMultiplier = cvar->FindVar("zoom_sensitivity_ratio_mouse")->GetFloat();

    Math::NormalizeAngles(delta);

    cmd->mousedx = -delta.y / (m_yaw * sens * zoomMultiplier);
    cmd->mousedy = delta.x / (m_pitch * sens * zoomMultiplier);
}


void Ragebot::CreateMove(CUserCmd* cmd)
{
	if (!Settings::Ragebot::enabled)
		return;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
		return;

    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon || activeWeapon->GetInReload())
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;
	
	if (Ragebot::prevWeapon != (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex())
	{
		Ragebot::prevWeapon = (ItemDefinitionIndex)*activeWeapon->GetItemDefinitionIndex();
		Ragebot::UpdateValues();
	}

    QAngle oldAngle;
    engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;

	Vector localEye = localplayer->GetEyePosition();
    QAngle angle = cmd->viewangles;

    Vector bestSpot = Vector(0);
	float bestDamage = 0.f;

	C_BasePlayer* player = nullptr;
	switch (Settings::Ragebot::enemySelectionType)
	{
		case EnemySelectionType::BestDamage :
			player = GetBestEnemyAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
		case EnemySelectionType::CLosestToCrosshair :
			player = GetClosestPlayerAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
		default:
			break;
	}

    if (player && bestDamage > 0.f)
    {	
		cvar->ConsoleDPrintf(XORSTR("found player\n"));
		if (Settings::Ragebot::AutoShoot::autoscope)
	    	if (Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) )
	    	{
				cmd->buttons |= IN_ATTACK2;
				return; // will go to the next tick
	    	}
		
		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.
		angle = Math::CalcAngle(localEye, bestSpot);

		RagebotAutoSlow(localplayer, activeWeapon, cmd, oldForward, oldSideMove);
    	RagebotAutoCrouch(player, cmd, activeWeapon);
    	RagebotAutoPistol(activeWeapon, cmd);
    	RagebotAutoShoot(player,localplayer, activeWeapon, cmd, bestSpot);
    	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon);
    }

    Math::NormalizeAngles(angle);
    // Math::ClampAngles(angle);

    FixMouseDeltas(cmd, player, angle, oldAngle);
    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	
    // if (!Settings::Ragebot::silent)
	// 	engine->SetViewAngles(cmd->viewangles);
}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if(!Settings::Ragebot::enabled || !event)
		return;
		
    if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    	return;
		Ragebot::friends.clear();
    }
    if (strcmp(event->GetName(), XORSTR("player_death")) == 0)
    {
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
	    	return;

		if (attacker_id != engine->GetLocalPlayer())
	    	return;

		RagebotkillTimes.push_back(Util::GetEpochTime());
    }
}

