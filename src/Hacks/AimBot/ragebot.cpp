// #pragma GCC diagnostic ignored "-Wcomment"
// #pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.hpp"
#include "resolver.h"
#include "../AntiAim/fakelag.h"
#include "../esp.h"
#include "aimbot.hpp"
#include <future>

#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )

#define TICK_INTERVAL			(globalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

QAngle RCSLastPunch;

static void VelocityExtrapolate(C_BasePlayer* player, Vector& aimPos)
{
	if (!player || !player->GetAlive())
		return;
	aimPos += (player->GetVelocity() * globalVars->interval_per_tick);
}

void Ragebot::BestHeadPoint(C_BasePlayer* player, const int &BoneIndex,int& Damage, Vector& Spot)
{
	matrix3x4_t matrix[128];

	if( !player->SetupBones(matrix, 128, 0x100, 0.f) )
		return;
	model_t *pModel = player->GetModel();
	if( !pModel )
		return;

	studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
	if( !hdr )
		return;
	mstudiobbox_t *bbox = hdr->pHitbox((int)Hitbox::HITBOX_HEAD, 0);
	if( !bbox )
		return;

	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);

	Vector center = ( mins + maxs ) * 0.5f;
	static Vector points[11] = 	{ 
									center, center, center, center, 
									center, center, center, center, 
									center,center,center
								};
	// 0 - center, 1 - forehead, 2 - skullcap, 3 - upperleftear, 4 - upperrightear, 5 - uppernose, 6 - upperbackofhead
	// 7 - leftear, 8 - rightear, 9 - nose, 10 - backofhead

	points[1].z += bbox->radius * 0.60f; // morph each point.
	points[2].z += bbox->radius * 1.25f; // ...
	points[3].x += bbox->radius * 0.80f;
	points[3].z += bbox->radius * 0.60f;
	points[4].x -= bbox->radius * 0.80f;
	points[4].z += bbox->radius * 0.90f;
	points[5].y += bbox->radius * 0.80f;
	points[5].z += bbox->radius * 0.90f;
	points[6].y -= bbox->radius * 0.80f;
	points[6].z += bbox->radius * 0.90f;
	points[7].x += bbox->radius * 0.80f;
	points[8].x -= bbox->radius * 0.80f;
	points[9].y += bbox->radius * 0.80f;
	points[10].y -= bbox->radius * 0.80f;

	for (int i = 0; i < 7; i++)
	{
		float bestDamage = AutoWall::GetDamage(points[i], true);
		if (bestDamage >= player->GetHealth())
		{
			Damage = bestDamage;
			Spot = points[i];
			return;
		}
		else if (bestDamage > Damage)
		{
		 	Damage = bestDamage;
			Spot = points[i];
		}
	}
}

void Ragebot::BestMultiPoint(C_BasePlayer* player,const int &BoneIndex,int& Damage, Vector& Spot)
{
	model_t* pModel = player->GetModel();
    if (!pModel)
		return;
    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		 return;
	mstudiobbox_t* bbox = hdr->pHitbox((int)BoneIndex, 0);
	if (!bbox)
		return;
	
	matrix3x4_t matrix[128];
	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, matrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, matrix[bbox->bone], maxs);
	// 0 - center 1 - left, 2 - right, 3 - back
	Vector center = ( mins + maxs ) * 0.5f;
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
		else if (bestDamage > Damage)
		{   
			Damage = bestDamage;
			Spot = points[i];
		}
	}
}

bool Ragebot::canShoot(C_BaseCombatWeapon* activeWeapon,Vector &bestSpot, C_BasePlayer* enemy,const RageWeapon_t& currentSettings)
{
	if (currentSettings.HitChance == 0)
		return false;
	if (!enemy || !enemy->GetAlive())
		return false;

    Vector src = localplayer->GetEyePosition();
    QAngle angle = Math::CalcAngle(src, bestSpot);
    Math::NormalizeAngles(angle);

	Vector forward, right, up;
    Math::AngleVectors(angle, &forward, &right, &up);

    static int hitCount;
	hitCount = 0;
    int NeededHits = static_cast<int>(2.56f * currentSettings.HitChance); // 256/100 = 2.56

    activeWeapon->UpdateAccuracyPenalty();
    float weap_spread = activeWeapon->GetSpread();
    float weap_inaccuracy = activeWeapon->GetInaccuracy();


	static double val1 = (2.0 * M_PI);
    for (int i = 0; i < 256; i++) {

		double b = RandomeFloat(val1);
        double spread = weap_spread * RandomeFloat(1.0f);
        double d = RandomeFloat(1.0f);
        double inaccuracy = weap_inaccuracy * RandomeFloat(1.0f);

        Vector spreadView((cos(b) * inaccuracy) + (cos(d) * spread), (sin(b) * inaccuracy) + (sin(d) * spread), 0), direction;

       	direction.x = forward.x + (spreadView.x * right.x) + (spreadView.y * up.x);
		direction.y = forward.y + (spreadView.x * right.y) + (spreadView.y * up.y);
		direction.z = forward.z + (spreadView.x * right.z) + (spreadView.y * up.z);
		direction.Normalize();
        	
		static QAngle viewAnglesSpread;
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

        if (static_cast<int>((hitCount/2.56)) >= currentSettings.HitChance) // 100/256 = 0.390625
			return true;

		if ((256 - i + hitCount) < NeededHits)
			return false;
    }

    return false;
}

void Ragebot::GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, const int& playerHelth,int& i,const std::unordered_map<int, int>* modelType)
{	

	static auto HitboxHead([&](int &BoneID){
		Spot = player->GetBonePosition(BoneID);
		BestHeadPoint(player, BoneID, Damage, Spot);
	});
	static auto UpperSpine([&](int &BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);

		if (Damage >= playerHelth)
			return;
			
		static const int BONE[] = 
								{	
									BONE_LEFT_COLLARBONE, 
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
				return;
			}
			if (bestDamage >= Damage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
		}
	});
	static auto MiddleSpine([&](int &BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= playerHelth)
			return;
			
		static const int BONE[] = {
									BONE_LEFT_ARMPIT,
    								BONE_RIGHT_ARMPIT,
									BONE_LEFT_BICEP,
    								BONE_RIGHT_BICEP,
									/*
									BONE_LEFT_ELBOW,
    								BONE_RIGHT_ELBOW*/
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
				return;
			}
			else if (bestDamage >= Damage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
		}
	});
	static auto LowerSpine([&](int &BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= playerHelth)
			return;
			
		static const int BONE[] = {	
									BONE_LEFT_FOREARM,
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
				return;
			}
			if (bestDamage >= Damage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
		}
	});
	static auto HipHitbox([&](int &BoneID){
		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);

		if (Damage >= playerHelth)
			return;
			
		static const int BONE[] = 
								{
									BONE_LEFT_BUTTCHEEK,
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
				return;
			}
			if (bestDamage >= Damage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
		}
	});
	static auto PelvisHitbox([&](int &BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		if (Damage >= playerHelth)
			return;
			
		static const int BONE[] = 
								{
									BONE_LEFT_KNEE, 
									BONE_LEFT_ANKLE,
									BONE_RIGHT_KNEE,
									BONE_RIGHT_ANKLE,
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
				return;
			}
			if (bestDamage >= Damage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				Damage = bestDamage;
				Spot = bone3D;
			}
		}
	});
	static auto DefaultHitbox([&](int &BoneID){
		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
	});

	int boneID = -1;

	switch (static_cast<DesireBones>(i))
	{
		case DesireBones::BONE_HEAD:
			boneID = (*modelType).at(BONE_HEAD);
			if (currentWeaponSetting->desiredMultiBones[i]) HitboxHead(boneID); // lamda expression because again creating a new method is going to make the source code mess :p
			else DefaultHitbox(boneID);
		break;
		
		case DesireBones::UPPER_CHEST:
			boneID = (*modelType).at(BONE_UPPER_SPINAL_COLUMN);
			if (currentWeaponSetting->desiredMultiBones[i]) UpperSpine(boneID);
			else DefaultHitbox(boneID);
		break;
			
		case DesireBones::MIDDLE_CHEST:
			boneID = (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN);
			if (currentWeaponSetting->desiredMultiBones[i]) MiddleSpine(boneID);
			else DefaultHitbox(boneID);
		break;
		
		case DesireBones::LOWER_CHEST:
			boneID = (*modelType).at(BONE_LOWER_SPINAL_COLUMN);
			if (currentWeaponSetting->desiredMultiBones[i]) LowerSpine(boneID);
			else DefaultHitbox(boneID);
		break;
		
		case DesireBones::BONE_HIP:
			boneID = (*modelType).at(BONE_HIP);
			if (currentWeaponSetting->desiredMultiBones[i]) HipHitbox(boneID);
			else DefaultHitbox(boneID);
		break;
		
		case DesireBones::LOWER_BODY:
			boneID = BONE_PELVIS;
			if (currentWeaponSetting->desiredMultiBones[i]) PelvisHitbox(boneID);
			else DefaultHitbox(boneID);
		break;
	}
}

void Ragebot::GetBestEnemy()
{
	static Vector bestSpot;
	static int bestDamage;
	bestSpot.Zero();
	bestDamage = 0;

	const int &maxClient = engine->GetMaxClients();
	C_BasePlayer* player = nullptr;
	static int boneIndex = 0;
	for (int i = 1; i < maxClient; ++i)
	{
		player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			i == engine->GetLocalPlayer() || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune() || 
			player->GetTeam() == localplayer->GetTeam() )
			continue;			

		if ( currentWeaponSetting->OnShot){
			if (currentWeaponSetting->OnShotOnKey ){
				if ( inputSystem->IsButtonDown(Settings::Ragebot::OnShotBtn ) ){
					if (player->GetIndex() != ShootEnemyIndex)
						continue;
				}
			}else {
				if (player->GetIndex() != ShootEnemyIndex)
					continue;
			}
		}			
			
		const int &playerHelth = player->GetHealth();
		const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

		for (boneIndex = 0; boneIndex < 6; boneIndex++)
		{
			if ( !currentWeaponSetting->desireBones[boneIndex] ) continue;

			if (boneIndex == 0 && player->GetHealth() < 70)
				boneIndex++;

			GetDamageAndSpots(player, bestSpot, bestDamage, playerHelth, boneIndex, modelType);

			if (bestDamage >= playerHelth)
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				enemy = player;
				return;
			}
			else if ( inputSystem->IsButtonDown(Settings::Ragebot::DamageOverrideBtn) ){
				if (currentWeaponSetting->DamageOverride > 0 && bestDamage >= currentWeaponSetting->DamageOverride && bestDamage > BestDamage ){
					BestDamage = bestDamage;
					BestSpot = bestSpot;
					enemy = player;
				}
				
			}
			else if (bestDamage > BestDamage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				enemy = player;
			}
		}
	}
}

void Ragebot::CheckHit()
{	
	if (!localplayer)
		return;
	if (!activeWeapon )
		return;

	auto traceEnd = bulPosition;
    Ray_t ray;
	AutoWall::FireBulletData fireBulletData;

	fireBulletData.src = Ragebot::data.PrevTickEyePosition;
    fireBulletData.filter.pSkip = localplayer;
	CCSWeaponInfo* weaponInfo = activeWeapon->GetCSWpnData();
	QAngle angles = Math::CalcAngle(fireBulletData.src, traceEnd);
	Math::AngleVectors(angles, fireBulletData.direction);
	Vector temp = fireBulletData.direction;
    
	fireBulletData.direction = temp.Normalize();
	fireBulletData.trace_length_remaining = weaponInfo->GetRange();
	fireBulletData.trace_length = 0;
	fireBulletData.penetrate_count = 4;
	fireBulletData.current_damage = (float) weaponInfo->GetDamage();

	while ( fireBulletData.penetrate_count > 0 && fireBulletData.current_damage >= 1.0f)
	{
		cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("round \n"));
		fireBulletData.trace_length_remaining = weaponInfo->GetRange() - fireBulletData.trace_length;
		Vector end = fireBulletData.src + fireBulletData.direction * fireBulletData.trace_length_remaining;
		
		ray.Init(fireBulletData.src, end + fireBulletData.direction * 40.f);
		trace->TraceRay(ray, MASK_SHOT, &fireBulletData.filter, &fireBulletData.enter_trace);
		
		if (fireBulletData.enter_trace.hitgroup < HitGroups::HITGROUP_GEAR && fireBulletData.enter_trace.hitgroup > HitGroups::HITGROUP_GENERIC)
		{
			cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("Hit something\n"));
			// cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("IHit Babe\n"));
			Ragebot::data.Hited = true;
			return;
		}

		if (!AutoWall::HandleBulletPenetration(weaponInfo, fireBulletData))
			break;
	}
    
	cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("Trace Compleate\n"));
}

void Ragebot::init(C_BasePlayer* _localplayer, C_BaseCombatWeapon* _activeWeapon)
{
	localplayer = _localplayer;
	activeWeapon = _activeWeapon;
	enemy = nullptr;
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

		angle.x -= NewPunch.x * 2.f;
		angle.y -= NewPunch.y * 2.f;
	}
	RCSLastPunch = CurrentPunch;
}

static void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!localplayer || !localplayer->GetAlive() || !currentSettings.AutoCroutch)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK | IN_BULLRUSH;
}

static void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, QAngle& angle, const RageWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow){
		Ragebot::data.autoslow = false;
		return;
	}		
	
	if (currentSettings.autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK)){ 
		cmd->buttons |= IN_ATTACK2; 
	}
		
	Ragebot::data.needToStop = true;
	
	static float speed = 0;

	float lenth = localplayer->GetVelocity().Length();
	if (lenth > speed){
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
	}else {
		if (cmd->forwardmove > 0)
			cmd->forwardmove = -1;
		else
			cmd->forwardmove = 1;
		if (cmd->sidemove > 0)
			cmd->sidemove = -1;
		else 
			cmd->sidemove = 1;
		// cmd->forwardmove *= -1;
		// cmd->sidemove *= -1;
	}
	
		
	speed = lenth;
	Ragebot::data.autoslow = true;
}

static void RagebotAutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
{
     if (!currentSettings.autoShootEnabled)
		return;

	if (!activeWeapon || activeWeapon->GetInReload())
		return;

    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        return;

    if(activeWeapon->GetAmmo() == 0)
        return;
    if (cmd->buttons & IN_USE)
        return;

    cmd->buttons |= IN_ATTACK;
  	float postponeFireReadyTime = activeWeapon->GetPostPoneReadyTime();
    if (postponeFireReadyTime > 0)
    {
        if (postponeFireReadyTime < globalVars->curtime)
        {
            if (player)
                return;
            cmd->buttons &= ~IN_ATTACK;
        }
    }
}

void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, const RageWeapon_t& currentSettings)
{
    if (!currentSettings.autoShootEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER){
		cmd->buttons |= IN_ATTACK;
		return;
	}
		

	bool canShoot;
	canShoot = false;
	if (currentSettings.hitchanceType == HitchanceType::Normal){
		canShoot = Aimbot::canShoot(localplayer, activeWeapon, currentSettings.HitChance);
	}else {
		canShoot = Ragebot::canShoot(activeWeapon, bestspot, player, currentSettings);
	}
	if ( canShoot )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;

		Ragebot::data.autoslow = false;

		return;
	}

	// static prevSpe
	RagebotAutoSlow(localplayer, activeWeapon, cmd, angle, currentSettings);
}

inline void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, const RageWeapon_t& currentSettings)
{
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	if (!currentSettings.autoPistolEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
    if (*activeWeapon->GetItemDefinitionIndex() != ItemDefinitionIndex::WEAPON_REVOLVER)
        cmd->buttons &= ~IN_ATTACK;
}

static void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, QAngle& angle, QAngle& oldAngle)
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

void Ragebot::CreateMove(CUserCmd* cmd)
{

	if (Ragebot::data.player && Ragebot::data.player->GetAlive() )
    {
		if (Ragebot::data.Hited && Ragebot::data.player->GetHealth() >= Ragebot::data.playerhelth){
            cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("say Miss Due To Resolver\n"));
			engine->ExecuteClientCmd("say Miss Due To Resolver");
		    Resolver::players[Ragebot::data.player->GetIndex()].MissedCount++;
        }
    }

	// memset(&data, 0, sizeof(DATA));
	data.player = nullptr;
	Ragebot::data.Hited = false;

	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
		return;
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
		return;

	if (!localplayer->GetAlive() || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime){
		if (cmd->buttons & IN_ATTACK)
			cmd->buttons &= ~IN_ATTACK;
		return;
	}

	init(localplayer, activeWeapon);

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end()){
		index = *activeWeapon->GetItemDefinitionIndex();
	}
	currentWeaponSetting = &Settings::Ragebot::weapons.at(index);

	static QAngle oldAngle;
    	engine->GetViewAngles(oldAngle);
	static float oldForward;
		oldForward = cmd->forwardmove;
    static float oldSideMove;
		oldSideMove = cmd->sidemove;
	static QAngle angle;
		angle = cmd->viewangles;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

	localEye = localplayer->GetEyePosition();
   	BestSpot.Zero();
	BestDamage = (int)0;
	ShootEnemyIndex = 0;

	GetBestEnemy();
	
    if (enemy && Ragebot::BestDamage > 0)
    {
		Settings::Debug::AutoAim::target = Ragebot::BestSpot;

		RagebotAutoShoot(enemy, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, *currentWeaponSetting);
		RagebotAutoCrouch(enemy, cmd, activeWeapon, *currentWeaponSetting);

		if (cmd->buttons & IN_ATTACK)
		{
			Ragebot::data.player = enemy;
			Ragebot::data.playerhelth = enemy->GetHealth();
			Ragebot::data.PrevTickEyePosition = Ragebot::localEye;
			Ragebot::data.shooted = true;
			if (Settings::AntiAim::InvertOnShoot) { Settings::AntiAim::inverted = !Settings::AntiAim::inverted; }
			VelocityExtrapolate(enemy, BestSpot);
			Math::CalcAngle(localEye, BestSpot, angle);
			cmd->sidemove = cmd->forwardmove = 0;
		}
    }
	
	RagebotAutoR8(enemy, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, *currentWeaponSetting);
	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon, *currentWeaponSetting);
	AutoPistol(activeWeapon, cmd, *currentWeaponSetting);

	Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    FixMouseDeltas(cmd, localplayer, angle, oldAngle);

    cmd->viewangles = angle;

	if (cmd->buttons & IN_ATTACK){
		if (!currentWeaponSetting->silent)
			engine->SetViewAngles(angle);
	}
}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if(!event)	
		return;
	
	if ( strcmp(event->GetName(), XORSTR("bullet_impact")) == 0){
		int UserID = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));
		if (  UserID == engine->GetLocalPlayer() ) {
			if (!Ragebot::data.shooted)
				return;
			else
				Ragebot::data.shooted = false;

			const float x = event->GetFloat(XORSTR("x")), 
						y = event->GetFloat(XORSTR("y")), 
						z = event->GetFloat(XORSTR("z"));
			bulPosition.Init(x, y, z);
			Ragebot::CheckHit();
		}else {
			ShootEnemyIndex = UserID;
		}
	}
	// else if (strcmp(event->GetName(), XORSTR("player_hurt")) == 0){
	// 	int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
	// 	if (attacker_id != engine->GetLocalPlayer())
	// 		return;
	// 	cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("I Hit\n"));
	// }
    
}

void Ragebot::FireGameEvent2(IGameEvent* event)
{
	if ( strcmp(event->GetName(), XORSTR("bullet_impact")) == 0 ){
		const float x = event->GetFloat(XORSTR("x")), 
					y = event->GetFloat(XORSTR("y")), 
					z = event->GetFloat(XORSTR("z"));

		ESP::bulletBeam.bulletPosition.push_front(Vector(x,y,z));
		ESP::bulletBeam.enemyIndex = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));
	}
	if (strcmp(event->GetName(), XORSTR("player_death")) == 0)
    {
		// cvar->ConsoleColorPrintf(ColorRGBA(255,200,156,255), XORSTR("player_death\n"));
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
	    	return;

		if (attacker_id != engine->GetLocalPlayer())
	    	return;
		// else the player we shoot is dead better to set it to 0
    }else if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    	return;
    }
}
