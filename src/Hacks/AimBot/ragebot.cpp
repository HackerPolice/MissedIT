#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.h"
#include "autowall.h"
#include "../antiaim.h"
#include "../lagcomp.h"

#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>

#define PI_F (3.14)
#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define TICK_INTERVAL globalVars->interval_per_tick


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

inline bool	doubleFire = false;
static QAngle RCSLastPunch = QAngle(0);

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos){	
	if (!player->GetAlive() || !player)
		return;
	aimPos += (player->GetVelocity().Length() * globalVars->interval_per_tick);	
}

static void BestMultiPointHEADDamage(C_BasePlayer* player, int BoneIndex, int& Damage, Vector& Spot, matrix3x4_t boneMatrix[])
{
	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;

	model_t* pModel = player->GetModel();
    if (!pModel)
		return;

    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return;

	mstudiobbox_t* bbox = hdr->pHitbox((int)BoneIndex, 0);
    
	if (!bbox)
		return;

	Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);
	// 0 - forehead, 1 - skullcap,
	// 2 - leftear, 3 - rightear, 4 - backofhead
	const Vector &center = ( mins + maxs ) * 0.5f;
	Vector points[] = {center,center,center,center};

    points[0].z += bbox->radius * 0.60f; // morph each point.
	points[0].z += bbox->radius * 1.25f; // ...
	points[1].y -= bbox->radius * 0.65f;
	points[1].z += bbox->radius * 0.90f;
	points[2].x += bbox->radius * 0.80f;
	points[3].x -= bbox->radius * 0.80f;
	points[4].y += bbox->radius * 0.80f;

	for (int i = 0; i < 4; i++)
	{
		AutoWall::FireBulletData data;
		const float &bestDamage = AutoWall::GetDamage(points[i],localplayer, true, data);
		if (bestDamage >= player->GetHealth())
		{
			Damage = bestDamage;
			Spot = points[i];
			return;
		}
		if (bestDamage > Damage)
		{
			Damage = bestDamage;
			Spot = points[i];
		}
	}
}

static void BestMultiPointDamage(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot, matrix3x4_t boneMatrix[])
{
	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return;
	// cvar->ConsoleDPrintf(XORSTR("SetupBones\n"));
	
	// cvar->ConsoleDPrintf(XORSTR("SetupModel\n"));
	model_t* pModel = player->GetModel();
    if (!pModel)
		return;
	// cvar->ConsoleDPrintf(XORSTR("Setuphdr\n"));
    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		return;
    // cvar->ConsoleDPrintf(XORSTR("SetupBbox\n"));
	mstudiobbox_t* bbox = hdr->pHitbox((int)BoneIndex, 0);
    
	if (!bbox)
		return;

    Vector mins, maxs;
    Math::VectorTransform(bbox->bbmin, boneMatrix[bbox->bone], mins);
    Math::VectorTransform(bbox->bbmax, boneMatrix[bbox->bone], maxs);

	// 0 - center 1 - left, 2 - right, 3 - back
	const Vector &center = ( mins + maxs ) * 0.5f;
	Vector points[3] = { center,center,center };

    points[1].x += bbox->radius * 0.80f; // morph each point.
	points[2].x -= bbox->radius * 0.80f;
	points[3].y -= bbox->radius * 0.80f;

	for (int i = 0; i < 3; i++)
	{
		AutoWall::FireBulletData data;
		const float &bestDamage = AutoWall::GetDamage(points[i],localplayer, true, data);
		if (bestDamage >= player->GetHealth())
		{
			Damage = bestDamage;
			Spot = points[i];
			return;
		}
		if (bestDamage > Damage)
		{
			Damage = bestDamage;
			Spot = points[i];
		}
		// cvar->ConsoleDPrintf(XORSTR("Best Damage : %d of bone ID : %d\n"), Damage, BoneIndex);
	}
}

static void GetDamageAndSpots(C_BasePlayer* player, C_BasePlayer* localplayer, Vector &Spot, int& Damage, const int& playerHelth, const int& i,const std::unordered_map<int, int>* modelType, matrix3x4_t boneMatrix[])
{
	if ( !localplayer || !player || !localplayer->GetAlive() || !player->GetAlive())
		return;
	if (!Settings::Ragebot::AutoAim::desireBones[i])	
		return;

	using namespace Settings::Ragebot::AutoAim;

	int boneID = -1;

	switch ((DesireBones)i)
	{
		case DesireBones::BONE_HEAD:
			boneID = CONST_BONE_HEAD;
			if ( playerHelth <= 90 )
				boneID = (*modelType).at(BONE_NECK);
			break;
		case DesireBones::UPPER_CHEST:
			if ( Damage >= 80)
				return;
			boneID = (*modelType).at(BONE_UPPER_SPINAL_COLUMN);
			break;
		case DesireBones::MIDDLE_CHEST:
			if ( Damage >= 80)
				return;
			boneID = (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN);
			break;
		case DesireBones::LOWER_CHEST:
			if ( Damage >= 80)
				return;
			boneID = (*modelType).at(BONE_LOWER_SPINAL_COLUMN);
			break;
		case DesireBones::BONE_HIP:
			if ( Damage > 40)
				return;
			boneID = (*modelType).at(BONE_HIP);
			break;
		case DesireBones::LOWER_BODY:
			if ( Damage > 40)
				return;
			boneID = BONE_PELVIS;
			break;
	}


	if (boneID == BONE_PELVIS && desiredMultiBones[i])
	{
		boneID = (*modelType).at(BONE_PELVIS);
		const Vector &bone3D = player->GetBonePosition(boneID);
		AutoWall::FireBulletData data;
		const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);

		if ( bestDamage >= 40)
		{
			Damage = bestDamage;
			Spot = bone3D;
			return;
		}

		for (int j = BONE_LEFT_BUTTCHEEK; j <= BONE_RIGHT_SOLE; j++)
		{
			boneID = (*modelType).at(j);
			const Vector &bone3D = player->GetBonePosition(boneID);
			const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 40)
				return;
		}
	}
	else if ( boneID == CONST_BONE_HEAD && desiredMultiBones[i])
	{
		const Vector &bone3D = player->GetBonePosition(boneID);
		AutoWall::FireBulletData data;
		const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);

		if ( bestDamage >= 90)
		{
			Damage = bestDamage;
			Spot = bone3D;
			return;
		}
		BestMultiPointHEADDamage(player, boneID, Damage, Spot, boneMatrix);
	}
	else if ( boneID == (*modelType).at(BONE_LOWER_SPINAL_COLUMN) && desiredMultiBones[i])
	{
		BestMultiPointDamage(player, boneID, Damage, Spot, boneMatrix);

		if (Damage >= 80)
			return;
		
		for (int j = BONE_LEFT_COLLARBONE; j <= BONE_RIGHT_SHOULDER; j++)
		{
			boneID = (*modelType).at(j);
			const Vector &bone3D = player->GetBonePosition(boneID);
			AutoWall::FireBulletData data;
			const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	}
	else if ( boneID == (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN) && desiredMultiBones[i])
	{
		BestMultiPointDamage(player, boneID, Damage, Spot, boneMatrix);

		if (Damage >= 80)
			return;
		
		for (int j = BONE_LEFT_ARMPIT; j <= BONE_RIGHT_ELBOW; j++)
		{
			boneID = (*modelType).at(j);
			const Vector &bone3D = player->GetBonePosition(boneID);
			AutoWall::FireBulletData data;
			const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	}
	else if ( boneID == (*modelType).at(BONE_LOWER_SPINAL_COLUMN) && desiredMultiBones[i])
	{
		BestMultiPointDamage(player, boneID, Damage, Spot, boneMatrix);

		if (Damage >= 80)
			return;
		
		for (int j = BONE_LEFT_FOREARM; j <= BONE_RIGHT_WRIST; j++)
		{
			boneID = (*modelType).at(j);
			const Vector &bone3D = player->GetBonePosition(boneID);
			AutoWall::FireBulletData data;
			const int &bestDamage = AutoWall::GetDamage(bone3D, localplayer, true, data);
			if (bestDamage >= playerHelth)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= Damage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
			if (bestDamage >= 70)
				return;
		}
	}
	else if ( desiredMultiBones[i])
		BestMultiPointDamage(player, boneID, Damage, Spot, boneMatrix);
	else
	{
		const Vector &bone3D = player->GetBonePosition(boneID);
		AutoWall::FireBulletData data;
		Damage = AutoWall::GetDamage(bone3D,localplayer, true, data);
		Spot = bone3D;
    }
}

static void GetBestSpotAndDamage(C_BasePlayer* player,C_BasePlayer* localplayer, Vector& Spot, int& Damage)
{	
	if (!player || !player->GetAlive())
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	// Atleast Now Total Bones we are caring of
	Vector spot = Vector(0);
	int damage = 0.f;

	const int &playerHelth = player->GetHealth();
	const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

	static matrix3x4_t boneMatrix[128];
	if ( !player->SetupBones(boneMatrix, 128, 0x100, 0) )
		return;

	if (Settings::Ragebot::damagePrediction == DamagePrediction::damage)
	{
		int i = 0;
		while ( i <= 5)
		{
			GetDamageAndSpots(player, localplayer, spot, damage, playerHelth, i, modelType, boneMatrix);
				
			if (damage >= playerHelth)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			else if (damage > Damage && damage > Settings::Ragebot::MinDamage)
			{
				Damage = damage;
				Spot = spot;
			}
			i++;
		}	
	}

	else if (Settings::Ragebot::damagePrediction == DamagePrediction::justDamage)
	{	
		int i = 0;
		while ( i <= 5)
		{
			GetDamageAndSpots(player, localplayer, spot, damage, playerHelth, i, modelType, boneMatrix);
				
			if (damage >= playerHelth)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			else if (damage > Damage && damage > Settings::Ragebot::MinDamage)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			i++;
		}		
	}

}

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

static C_BasePlayer* GetClosestPlayerAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& BestSpot, int& BestDamage)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;
	
	Vector bestSpot = Vector(0);
	int bestDamage = 0;

	if (Ragebot::LockedEnemy)
	{
		GetBestSpotAndDamage(Ragebot::LockedEnemy,localplayer, bestSpot, bestDamage);
		if (bestDamage >= Ragebot::LockedEnemy->GetHealth())
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return Ragebot::LockedEnemy;
		}
		else if (bestDamage >= Settings::Ragebot::MinDamage)
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return Ragebot::LockedEnemy;
		}
	}
	
	C_BasePlayer* clossestEnemy = nullptr;
	for (int i = engine->GetMaxClients(); i > 1 ; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune())
			continue;

		if (Entity::IsTeamMate(player, localplayer)) // Checking for Friend If any it will continue to next player
			continue;
		if (Ragebot::LockedEnemy)
			if (player == Ragebot::LockedEnemy)
				continue;

		GetBestSpotAndDamage(player,localplayer, bestSpot, bestDamage);
		
		if (bestDamage >= player->GetHealth() )
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return player;
		}	
		else if (bestDamage > BestDamage){
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			clossestEnemy = player;
		}
	}	

	if (bestSpot.IsZero())
		return nullptr;

	return clossestEnemy;
}

static C_BasePlayer* GetBestEnemyAndSpot(CUserCmd* cmd,C_BasePlayer* localplayer, Vector& BestSpot, int& BestDamage)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;
	
	Vector bestSpot = Vector(0);
	int bestDamage = 0;

	if (Ragebot::LockedEnemy)
	{
		GetBestSpotAndDamage(Ragebot::LockedEnemy,localplayer, bestSpot, bestDamage);
		if (bestDamage >= Ragebot::LockedEnemy->GetHealth())
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return Ragebot::LockedEnemy;
		}
		else if (bestDamage >= Settings::Ragebot::MinDamage)
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return Ragebot::LockedEnemy;
		}
	}
	
	C_BasePlayer* clossestEnemy = nullptr;
	for (int i = 2; i <= engine->GetMaxClients(); i++)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune())
			continue;

		if (Entity::IsTeamMate(player, localplayer)) // Checking for Friend If any it will continue to next player
			continue;
		if (Ragebot::LockedEnemy)
		{
			Ragebot::LockedEnemy = nullptr;
			if (player == Ragebot::LockedEnemy)
				continue;
		}
			

		GetBestSpotAndDamage(player,localplayer, bestSpot, bestDamage);
		
		if (bestDamage >= player->GetHealth() )
		{
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			return player;
		}	
		else if (bestDamage > BestDamage){
			BestDamage = bestDamage;
			BestSpot = bestSpot;
			clossestEnemy = player;
		}
	}	

	if (bestSpot.IsZero())
		return nullptr;

	return clossestEnemy;
}
/*
static bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
	if(!localplayer || !localplayer->GetAlive() || !activeWeapon || activeWeapon->GetInReload())
		return false;
	if (!Settings::Ragebot::HitChance::enabled)
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
	
	return hitchance >= (Settings::Ragebot::HitChance::value*2);
}
*/

static bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon,const Vector &bestSpot, C_BasePlayer* enemy)
{
    Vector src = localplayer->GetEyePosition();
    QAngle angle = Math::CalcAngle(src, bestSpot);
    Math::NormalizeAngles(angle);

	Vector forward, right, up;
    Math::AngleVectors(angle, &forward, &right, &up);

    int hitCount = 0;
    int NeededHits = static_cast<int>(255.f * (Settings::Ragebot::HitChance::value / 100.f));

    activeWeapon->UpdateAccuracyPenalty();
    float weap_spread = activeWeapon->GetSpread();
    float weap_inaccuracy = activeWeapon->GetInaccuracy();

    for (int i = 0; i < 255; i++) {

        float b = static_cast<float>( static_cast<float>(std::rand())/ static_cast<float>(RAND_MAX/ (2.0 * (float)M_PI)));
        float spread = weap_spread * static_cast<float>( static_cast<float>(std::rand())/ static_cast<float>(RAND_MAX/1.0));
        float d = static_cast<float>( static_cast<float>(std::rand())/ static_cast<float>(RAND_MAX/1.0));
        float inaccuracy = weap_inaccuracy * static_cast<float>( static_cast<float>(std::rand())/ static_cast<float>(RAND_MAX/1.0));

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

        if (static_cast<int>((static_cast<float>(hitCount) / 255.f) * 100.f) >= Settings::Ragebot::HitChance::value)
			return true;

		if ((255 - i + hitCount) < NeededHits)
			return false;
    }

    return false;
}


static void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon)
{
    if (!(cmd->buttons & IN_ATTACK) 
		|| !localplayer
		|| !localplayer->GetAlive())
		return;
	
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
		return;

	QAngle CurrentPunch = *localplayer->GetAimPunchAngle();

	if (Settings::Ragebot::silent)
	{
		angle.x -= CurrentPunch.x * 2.f;
		angle.y -= CurrentPunch.y * 2.f;
	}
	else if (localplayer->GetShotsFired() > 1 )
	{
		const QAngle &NewPunch = { CurrentPunch.x - RCSLastPunch.x, CurrentPunch.y - RCSLastPunch.y, 0 };

		angle.x -= NewPunch.x * 2.0;
		angle.y -= NewPunch.y * 2.0;
	}
	RCSLastPunch = CurrentPunch;
}

static void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon)
{
    if (!localplayer || !localplayer->GetAlive() || !Settings::Ragebot::AutoCrouch::enable)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK | IN_BULLRUSH;
}

static void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle)
{
	if (!Settings::Ragebot::AutoSlow::enabled)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (Settings::Ragebot::AutoShoot::autoscope && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) )
	{
		cmd->buttons |= IN_ATTACK2;
		return;
	}

	cvar->ConsoleDPrintf(XORSTR("IN AutoSlow Mode\n"));
	QAngle ViewAngle = cmd->viewangles;
	Math::ClampAngles(ViewAngle);
	static Vector oldOrigin = localplayer->GetAbsOrigin();
	Vector velocity = (localplayer->GetVecOrigin()-oldOrigin) * (1.f/globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin();
	const float &speed  = velocity.Length();
		
	if(localplayer->GetVelocity().Length() > 15.f)
	{
		QAngle dir;
		Math::VectorAngles(velocity, dir);
		dir.y = ViewAngle.y - dir.x;
		
		Vector NewMove = Vector(0);
		Math::AngleVectors(dir, NewMove);
		const auto &max = std::max(forrwordMove, sideMove);
		const auto &mult = 450.f/max;
		NewMove *= -mult;
			
		forrwordMove = NewMove.x;
		sideMove = NewMove.y;
	}
	// else 
	// {	
	// 	forrwordMove = 0.f;
	// 	sideMove = 0.f;
	// }
	
	cmd->buttons |= IN_WALK;
	Ragebot::shouldSlow = true;
}

static void RagebotAutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove)
{
    if (!Settings::Ragebot::AutoShoot::enabled)
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
			if ( !canShoot(cmd, localplayer, activeWeapon, bestspot, player) <= Settings::Ragebot::HitChance::value * 1.5)
			{
				RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle);
				cmd->buttons &= ~IN_ATTACK;
			}	

			
		}
		else if (postponeFireReadyTime < globalVars->curtime )
			cmd->buttons &= ~IN_ATTACK;

		return;
	}
}

static void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,const Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove)
{
    if (!Settings::Ragebot::AutoShoot::enabled)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (!activeWeapon)
		return;
	// cvar->ConsoleDPrintf(XORSTR("AutoShooting \n"));
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
		return;
		
	if ( canShoot(cmd, localplayer, activeWeapon, bestspot, player) )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		{
			Ragebot::shouldAim = false;
			cmd->buttons &= ~IN_ATTACK;
		}
		else
		{
			Ragebot::shouldAim = true;
			cmd->buttons |= IN_ATTACK;
		}
		cvar->ConsoleDPrintf(XORSTR("SHooting \n"));
		return;
	}
	cvar->ConsoleDPrintf(XORSTR("can't shoot SHoot \n"));	
	RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle);
		
}

static void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, const QAngle& angle, const QAngle& oldAngle)
{
    if (!player)
		return;
	if (!player->GetAlive())
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
	// ( activeWeapon->GetNextPrimaryAttack() > globalVars->curtime && localplayer->GetShotsFired() > 1)
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;
	
	QAngle oldAngle;
    engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
	QAngle angle = cmd->viewangles;

	// if (Ragebot::shouldSlow)
	// {
	// 	if ( !canShoot(cmd, localplayer, activeWeapon, bestspo) )
	// 	{
	// 		RagebotAutoSlow(localplayer, activeWeapon, cmd, oldForward, oldSideMove, angle);
	// 		Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	// 		return;
	// 	}
	// 	else{
	// 		Ragebot::shouldSlow = false;
	// 	}
	// }
	
	Ragebot::UpdateValues();
	Vector localEye = localplayer->GetEyePosition();
    Vector bestSpot = Vector(0);
	int bestDamage = 0;

	C_BasePlayer* player = nullptr;
	switch (Settings::Ragebot::enemySelectionType)
	{
		case EnemySelectionType::BestDamage :
			player = GetBestEnemyAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
		case EnemySelectionType::CLosestToCrosshair :
			player = GetClosestPlayerAndSpot(cmd, localplayer, bestSpot, bestDamage);
			break;
	}

    if (player && bestDamage > 0.f)
    {
		Ragebot::LockedEnemy = player;
		// cvar->ConsoleDPrintf(XORSTR("Player Found"));
		Settings::Debug::AutoAim::target = bestSpot;

		VelocityExtrapolate(player, localEye);
		VelocityExtrapolate(player, bestSpot);

    	RagebotAutoShoot(player, localplayer, activeWeapon, cmd, bestSpot, angle, oldForward, oldSideMove);
    	RagebotAutoR8(player, localplayer, activeWeapon, cmd, bestSpot, angle, oldForward, oldSideMove);
		RagebotAutoCrouch(player, cmd, activeWeapon);

		if (cmd->buttons & IN_ATTACK)
		{
			angle = Math::CalcAngle(localEye, bestSpot);
			CreateMove::sendPacket = true;
		}
    }
	else{
		Ragebot::shouldAim = false;
		Ragebot::LockedEnemy = nullptr;
		Ragebot::shouldSlow = false;
	}
	
	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon);
    Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    FixMouseDeltas(cmd, player, angle, oldAngle);
    cmd->viewangles = angle;

	if (!Settings::Ragebot::silent)
		engine->SetViewAngles(cmd->viewangles);

	Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	
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

