#pragma GCC diagnostic ignored "-Wcomment"
#pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.h"


#define PI_F (3.14)
#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define TICK_INTERVAL globalVars->interval_per_tick
#define RandomeFloat(x) (static_cast<float>( static_cast<float>(std::rand())/ static_cast<float>(RAND_MAX/x)))
#define TIME_TO_TICKS( dt )	( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )




std::vector<int64_t> Ragebot::friends = {};
std::vector<long> RagebotkillTimes = { 0 }; // the Epoch time from when we kill someone

inline bool	doubleFire = false;
static QAngle RCSLastPunch = QAngle(0);

static Ragebot::enemy lockedEnemy;

static void BestMultiPointHEADDamage(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot)
{
	if (!player || !player->GetAlive())
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

	// 0 - center, 1 - skullcap,
	// 2 - leftear, 3 - rightear, 4 - backofhead
	Vector center = Spot;
	Vector points[5] = {center,center,center,center,center};

	points[1].y -= bbox->radius * 0.65f;
	points[1].z += bbox->radius * 0.90f;
	points[2].x += bbox->radius * 0.80f;
	points[3].x -= bbox->radius * 0.80f;
	points[4].y += bbox->radius * 0.80f;

	for (int i = 0; i < 5; i++)
	{
		float bestDamage = AutoWall::GetDamage(points[i], true);
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

static void BestMultiPointDamage(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot)
{
	if (!player || !player->GetAlive())
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

	// 0 - center 1 - left, 2 - right, 3 - back
	Vector center = Spot;
	Vector points[4] = { center,center,center,center };

    points[1].x += bbox->radius * 0.95f; // morph each point.
	points[2].x -= bbox->radius * 0.95f;
	points[3].y -= bbox->radius * 0.95f;

	for (int i = 0; i < 4; i++)
	{
		
		int bestDamage = AutoWall::GetDamage(points[i], true);
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

static void GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, int& playerHelth, int& i,const std::unordered_map<int, int>* modelType, const RageWeapon_t& currentSetting)
{
	if (!player || !player->GetAlive() || !currentSetting.desireBones[i])
		return;

	auto HitboxHead([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		BestMultiPointHEADDamage(player, BoneID, Damage, Spot);
	});
	auto UpperSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {	BONE_LEFT_COLLARBONE, 
									BONE_RIGHT_COLLARBONE,
									BONE_LEFT_SHOULDER, 
									BONE_RIGHT_SHOULDER
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
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
	});
	auto MiddleSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {BONE_LEFT_ARMPIT,
    								BONE_RIGHT_ARMPIT,
									BONE_LEFT_BICEP,
    								BONE_RIGHT_BICEP,
									BONE_LEFT_ELBOW,
    								BONE_RIGHT_ELBOW};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
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
	});
	auto LowerSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 80 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {	BONE_LEFT_FOREARM,
    								BONE_LEFT_WRIST,
    								BONE_RIGHT_FOREARM,
    								BONE_RIGHT_WRIST,
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
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
	});
	auto HipHitbox([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 40 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {BONE_LEFT_BUTTCHEEK,
    								BONE_LEFT_THIGH,
    								BONE_RIGHT_BUTTCHEEK,
    								BONE_RIGHT_THIGH,
								};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
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
	});
	auto PelvisHitbox([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= 40 || Damage >= playerHelth)
			return;
			
		static const int BONE[] = {BONE_LEFT_KNEE, 
									BONE_LEFT_ANKLE,
									BONE_LEFT_SOLE, 
									BONE_RIGHT_BUTTCHEEK,
									BONE_RIGHT_THIGH,
									BONE_RIGHT_KNEE,
									BONE_RIGHT_ANKLE,
									BONE_RIGHT_SOLE};
		
		for (auto &j : BONE)
		{
			BoneID = (*modelType).at(j);
			Vector bone3D = player->GetBonePosition(BoneID);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
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
	});
	auto DefaultHitbox([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
	});

	int boneID = -1;

	switch (static_cast<DesireBones>(i))
	{
		case DesireBones::BONE_HEAD:
			boneID = (*modelType).at(BONE_HEAD);
			if ( playerHelth <= 90  ) boneID = (*modelType).at(BONE_NECK);
			if (currentSetting.desiredMultiBones[i]) HitboxHead(boneID); // lamda expression because again creating a new method is going to make the source code mess :p
			else DefaultHitbox(boneID);
			break;
		
		case DesireBones::UPPER_CHEST:
			boneID = (*modelType).at(BONE_UPPER_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) UpperSpine(boneID);
			else DefaultHitbox(boneID);
			break;
			

		case DesireBones::MIDDLE_CHEST:
			boneID = (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) MiddleSpine(boneID);
			else DefaultHitbox(boneID);
			break;
		
		case DesireBones::LOWER_CHEST:
			boneID = (*modelType).at(BONE_LOWER_SPINAL_COLUMN);
			if (currentSetting.desiredMultiBones[i]) LowerSpine(boneID);
			else DefaultHitbox(boneID);
			break;
		
		case DesireBones::BONE_HIP:
			boneID = (*modelType).at(BONE_HIP);
			if (currentSetting.desiredMultiBones[i]) HipHitbox(boneID);
			else DefaultHitbox(boneID);
			break;
		
		case DesireBones::LOWER_BODY:
			boneID = BONE_PELVIS;
			if (currentSetting.desiredMultiBones[i]) PelvisHitbox(boneID);
			else DefaultHitbox(boneID);
			break;
	}
}

static void GetBestSpotAndDamage(C_BasePlayer* player,C_BasePlayer* localplayer, Vector& Spot, int& Damage,const RageWeapon_t& currSettings)
{	
	if (!player || !localplayer || !player->GetAlive() || !localplayer->GetAlive())
		return;
		
	// Atleast Now Total Bones we are caring of
	Vector spot = Vector(0);
	int damage = 0.f;

	int playerHelth = player->GetHealth();
	const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

	// static matrix3x4_t boneMatrix[128];
	// 	player->SetupBones(boneMatrix, 128, 0x100, 0);

	if (currSettings.DmagePredictionType == DamagePrediction::damage)
	{
		int i = 0;
		while ( i < 6)
		{
			GetDamageAndSpots(player, spot, damage, playerHelth, i, modelType, currSettings);
			if (damage >= playerHelth)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			else if (damage > Damage)
			{
				Damage = damage;
				Spot = spot;
			}
			i++;
		}	
	}

	else if (currSettings.DmagePredictionType == DamagePrediction::justDamage)
	{	
		int i = 0;

		if (player)
		while ( i < 6)
		{
			GetDamageAndSpots(player, spot, damage, playerHelth, i, modelType, currSettings);
				
			if (damage >= playerHelth)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			else if (damage > Damage)
			{
				Damage = damage;
				Spot = spot;
				return;
			}
			i++;
		}		
	}

}

/*
* To find the closesnt enemy to reduce the calculation time and increase performace
* Original Credits to: https://github.com/goldenguy00 ( study! study! study! :^) ) 
*/
static C_BasePlayer* GetClosestEnemy (C_BasePlayer *localplayer)
{
	if (!localplayer || !localplayer->GetAlive()) return nullptr;

	C_BasePlayer* closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	int maxClient = engine->GetMaxClients();

	for (int i = maxClient; i > 0; i--)
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

/*
 * Logic is simple as that we look for clossest enemt to your crosshair first 
 * then check for him first else cheking for other fellos 
 * though it can be costly sometimes but can be usefull in most of the cases
 */
static C_BasePlayer* GetClosestPlayerAndSpot(C_BasePlayer* localplayer, const RageWeapon_t& currSettings)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;
	
	Vector bestSpot = Vector(0);
	int bestDamage = 0;

	while (lockedEnemy.player)
	{
		if ( !lockedEnemy.player->GetAlive() || lockedEnemy.player->GetDormant() || lockedEnemy.player->GetImmune() )
			break;
		GetBestSpotAndDamage(lockedEnemy.player,localplayer, bestSpot, bestDamage, currSettings);
		if (bestDamage >= lockedEnemy.player->GetHealth() || bestDamage >= currSettings.MinDamage)
		{
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			return lockedEnemy.player;
		}
		lockedEnemy.player = nullptr;
		break;
	}
	
	C_BasePlayer* clossestEnemy = GetClosestEnemy(localplayer);

	if (clossestEnemy)
	{
		if (clossestEnemy->GetAlive() || !clossestEnemy->GetDormant() || !clossestEnemy->GetImmune())
		{
			GetBestSpotAndDamage(clossestEnemy,localplayer, bestSpot, bestDamage, currSettings);
		
			if (bestDamage >= clossestEnemy->GetHealth() || bestDamage > Ragebot::BestDamage)
			{
				Ragebot::BestDamage = bestDamage;
				Ragebot::BestSpot = bestSpot;
				return clossestEnemy;
			}	
		}
	}
	int maxClient = engine->GetMaxClients();
	for (int i = maxClient; i > 0 ; i--)
	{
		C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune() ||
			player == clossestEnemy)
			continue;

		if (Entity::IsTeamMate(player, localplayer)) // Checking for Friend If any it will continue to next player
			continue;			

		GetBestSpotAndDamage(player,localplayer, bestSpot, bestDamage, currSettings);
		
		if (bestDamage >= player->GetHealth() )
		{
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			return player;
		}	
		else if (bestDamage > Ragebot::BestDamage){
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			clossestEnemy = player;
		}
	}
	if (Ragebot::BestSpot.IsZero() || Ragebot::BestDamage < currSettings.MinDamage)
		return nullptr;

	return clossestEnemy;
}

/*
 * This player finding technique is bit slow because it look over all the fellos and there damages
 * it can cause fps drop too but usefull when someone try to kill you from behind if only they miss though
 */
static C_BasePlayer* GetBestEnemyAndSpot(C_BasePlayer* localplayer,const RageWeapon_t& currSettings)
{
	if (!localplayer || !localplayer->GetAlive())
		return nullptr;
	
	Vector bestSpot = Vector(0);
	int bestDamage = 0;

	while (lockedEnemy.player )
	{
		if (!lockedEnemy.player->GetAlive() || lockedEnemy.player->GetDormant() || lockedEnemy.player->GetImmune())
			break;
		GetBestSpotAndDamage(lockedEnemy.player,localplayer, bestSpot, bestDamage, currSettings);
		if (bestDamage >= lockedEnemy.player->GetHealth() || bestDamage >= currSettings.MinDamage)
		{
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			return lockedEnemy.player;
		}
		lockedEnemy.player = nullptr;
		lockedEnemy.bestDamage = 0;
		lockedEnemy.lockedSpot = Vector(0);
		break;
	}
	
	C_BasePlayer* clossestEnemy = nullptr;
	int maxClient = engine->GetMaxClients();
	for (int i = 0; i <= maxClient; ++i)
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

		GetBestSpotAndDamage(player,localplayer, bestSpot, bestDamage, currSettings);
		
		if (bestDamage >= player->GetHealth() )
		{
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			return player;
		}	
		else if (bestDamage > Ragebot::BestDamage){
			Ragebot::BestDamage = bestDamage;
			Ragebot::BestSpot = bestSpot;
			clossestEnemy = player;
		}
	}	

	if (Ragebot::BestSpot.IsZero() || Ragebot::BestDamage < currSettings.MinDamage)
		return nullptr;

	return clossestEnemy;
}

static bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon,Vector &bestSpot, C_BasePlayer* enemy,const RageWeapon_t& currentSettings)
{
	if (!localplayer || !localplayer->GetAlive())
		return false;

	if (!activeWeapon || activeWeapon->GetInReload())
		return false;

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
    	// RandomSeed(i + 1); // if we can't calculate spread like game does, then at least use same functions XD
        static float val1 = (2.0 * (float)M_PI);
		float b = RandomeFloat(val1);
        float spread = weap_spread * RandomeFloat(1.0f);
        float d = RandomeFloat(1.0f);
        float inaccuracy = weap_inaccuracy * RandomeFloat(1.0f);

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

        

		if ((255 - i + hitCount) < NeededHits)
			return false;
    }

	if (static_cast<int>((static_cast<float>(hitCount) / 255.f) * 100.f) >= currentSettings.HitChance)
		return true;

    return false;
}

static void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
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

static void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!localplayer || !localplayer->GetAlive() || !Settings::Ragebot::AutoCrouch::enable)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK;
}

static void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle, const RageWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (currentSettings.autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK) )
	{
		cmd->buttons |= IN_ATTACK2;
		Ragebot::shouldSlow = true;
		return;
	}

	QAngle ViewAngle;
		engine->GetViewAngles(ViewAngle);
	static Vector oldOrigin = localplayer->GetAbsOrigin();
	Vector velocity = (localplayer->GetVecOrigin()-oldOrigin) * (1.f/globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin();
	float speed  = velocity.Length();
		
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
	Ragebot::shouldSlow = true;
}

static void RagebotAutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
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
			if ( !canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings))
			{
				RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
				cmd->buttons &= ~IN_ATTACK;
			}	

			
		}
		else if (postponeFireReadyTime < globalVars->curtime )
			cmd->buttons &= ~IN_ATTACK;

		return;
	}
}

static void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
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
		
	if ( canShoot(cmd, localplayer, activeWeapon, bestspot, player, currentSettings) )
	{
		Ragebot::shouldAim = false;
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;
		return;
	}
	Ragebot::shouldAim = true;
	RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
		
}

static void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player,QAngle& angle,QAngle& oldAngle)
{
    if (!player || !player->GetAlive())
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
	
	QAngle oldAngle;
    	engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
	QAngle angle = cmd->viewangles;

	// if (Ragebot::shouldSlow)
	// {
	// 	if ( !canShoot(cmd, localplayer, activeWeapon, lockedEnemy.lockedSpot, lockedEnemy.player) )
	// 	{
	// 		RagebotAutoSlow(localplayer, activeWeapon, cmd, oldForward, oldSideMove, angle);
	// 		Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
	// 		return;
	// 	}
	// 	else{
	// 		Ragebot::shouldSlow = false;
	// 	}
	// }
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end())
		  index = *activeWeapon->GetItemDefinitionIndex();
    const RageWeapon_t& currentWeaponSetting = Settings::Ragebot::weapons.at(index);
		

	Ragebot::localEye = localplayer->GetEyePosition();
    Ragebot::BestSpot = Vector(0);
	Ragebot::BestDamage = 0;

	C_BasePlayer* player = nullptr;
	switch (currentWeaponSetting.enemySelectionType)
	{
		case EnemySelectionType::BestDamage :
			player = GetBestEnemyAndSpot(localplayer, currentWeaponSetting);
			break;
		case EnemySelectionType::CLosestToCrosshair :
			player = GetClosestPlayerAndSpot(localplayer, currentWeaponSetting);
			break;
	}

    if (player && Ragebot::BestDamage > 0.f)
    {
		lockedEnemy.player = player;
		lockedEnemy.lockedSpot = Ragebot::BestSpot;
		lockedEnemy.bestDamage = Ragebot::BestDamage;
		Settings::Debug::AutoAim::target = Ragebot::BestSpot;

		RagebotAutoShoot(player, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, currentWeaponSetting);
    	RagebotAutoR8(player, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, currentWeaponSetting);
		RagebotAutoCrouch(player, cmd, activeWeapon, currentWeaponSetting);

		if (cmd->buttons & IN_ATTACK)
		{
			angle = Math::CalcAngle(localEye, Ragebot::BestSpot);
			CreateMove::sendPacket = true;
		}
    }
	else{
		Ragebot::shouldAim = false;
		lockedEnemy.player = nullptr;
		lockedEnemy.lockedSpot = Vector(0);
		lockedEnemy.bestDamage = 0;
		Ragebot::shouldSlow = false;
	}
	
	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon, currentWeaponSetting);
	
    Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    FixMouseDeltas(cmd, player, angle, oldAngle);
    cmd->viewangles = angle;

	if (!currentWeaponSetting.silent)
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
