// #pragma GCC diagnostic ignored "-Wcomment"
// #pragma GCC diagnostic ignored "-Warray-bounds"

#include "ragebot.hpp"
#include "resolver.h"

#define absolute(x) ( x = x < 0 ? x * -1 : x)
#define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
#define NormalizeNo(x) (x = (x < 0) ? ( x * -1) : x )

#define TICK_INTERVAL			(globalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

QAngle RCSLastPunch;

/*
 * Class for damage and emeny prediction for ragebot
 * Decleard in ragebot.hpp
 */
void Ragebot::BestHeadPoint(C_BasePlayer* player, const int &BoneIndex, int& Damage, Vector& Spot)
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

void Ragebot::BestMultiPoint(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot)
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

        if (static_cast<int>((hitCount/2.56)) >= currentSettings.HitChance) // 100/256 = 0.390625
			return true;

		if ((256 - i + hitCount) < NeededHits)
			return false;
    }

    return false;
}

void Ragebot::GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, int& playerHelth, int& i,const std::unordered_map<int, int>* modelType)
{	

	static auto HitboxHead([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		BestHeadPoint(player, BoneID, Damage, Spot);
	});
	static auto UpperSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		// BestMultiPoint(player, BoneID, Damage, Spot);

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
	static auto MiddleSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		// BestMultiPoint(player, BoneID, Damage, Spot);
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
	static auto LowerSpine([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		// BestMultiPoint(player, BoneID, Damage, Spot);
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
	static auto HipHitbox([&](int BoneID){
		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		// BestMultiPoint(player, BoneID, Damage, Spot);

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
	static auto PelvisHitbox([&](int BoneID){

		Spot = player->GetBonePosition(BoneID);
		Damage = AutoWall::GetDamage(Spot, true);
		// BestMultiPoint(player, BoneID, Damage, Spot);
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
	static auto DefaultHitbox([&](int BoneID){
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
	enemy = nullptr;
	Vector bestSpot;
	bestSpot.Zero();
	int bestDamage;
	bestDamage = 0;

	const int &maxClient = engine->GetMaxClients();
	C_BasePlayer* player;
	player = nullptr;
	static int boneIndex;
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

		int playerHelth = player->GetHealth();
		const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

		for (boneIndex = 0; boneIndex < 6; boneIndex++)
		{
			if ( !currentWeaponSetting->desireBones[boneIndex] ) continue;

			GetDamageAndSpots(player, bestSpot, bestDamage, playerHelth, boneIndex, modelType);

			if (bestDamage >= playerHelth)
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				enemy = player;
				return;
			}
			else if (bestDamage > BestDamage && bestDamage >= currentWeaponSetting->MinDamage)
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				enemy = player;
			}
		}
	}

	if (inputSystem->IsButtonDown(Settings::Ragebot::DamageOverrideBtn) && BestDamage < currentWeaponSetting->DamageOverride){
		enemy = nullptr;
		BestDamage = 0;
	}
}

void Ragebot::CheckHit()
{
	if (!Ragebot::data.player)
		return;
	if (!Ragebot::data.shooted || !Ragebot::data.player->GetAlive())
		return;
	if (Ragebot::data.playerhelth > Ragebot::data.player->GetHealth())
		return;
	// cvar->ConsoleDPrintf(XORSTR("chcck for empty\n"));
	if (bulPosition.IsZero())
		return;
	auto traceEnd = bulPosition;
	// cvar->ConsoleDPrintf(XORSTR("not empty\n"));
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;
	
    Ray_t ray;
	AutoWall::FireBulletData data;

	data.src = Ragebot::data.PrevTickEyePosition;
    data.filter.pSkip = localplayer;
	CCSWeaponInfo* weaponInfo = activeWeapon->GetCSWpnData();
	QAngle angles = Math::CalcAngle(data.src, traceEnd);
	Math::AngleVectors(angles, data.direction);
	Vector temp = data.direction;
    
	data.direction = temp.Normalize();
	data.trace_length_remaining = weaponInfo->GetRange();
	data.trace_length = 0;
	data.penetrate_count = 4;
	data.current_damage = (float) weaponInfo->GetDamage();
	
	while ( data.penetrate_count > 0 && data.current_damage >= 1.0f)
	{
		data.trace_length_remaining = weaponInfo->GetRange() - data.trace_length;
		Vector end = data.src + data.direction * data.trace_length_remaining;

		AutoWall::TraceLine(data.src, end, MASK_SHOT, localplayer, &data.enter_trace);
		ray.Init(data.src, end + data.direction * 40.f);
		trace->TraceRay(ray, MASK_SHOT, &data.filter, &data.enter_trace);
		AutoWall::TraceLine(data.src, end + data.direction * 40.f, MASK_SHOT, localplayer, &data.enter_trace);

		if (data.enter_trace.hitgroup <= HitGroups::HITGROUP_RIGHTLEG && data.enter_trace.hitgroup > HitGroups::HITGROUP_GENERIC)
		{
			C_BasePlayer* player = (C_BasePlayer*) data.enter_trace.m_pEntityHit;

			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= powf(weaponInfo->GetRangeModifier(), data.trace_length * 0.002f);
			if ( player == Ragebot::data.player){
				engine->ExecuteClientCmd("say Miss Due To Resolver");
				Resolver::players[Ragebot::data.player->GetIndex()].MissedCount++;
			}
			else if (player) {
				engine->ExecuteClientCmd("say Ok I am on drugs || I fuking fuck some one elses ass shit :/");
			}
			bulPosition.Zero();
			return;
		}

		if (!AutoWall::HandleBulletPenetration(weaponInfo, data))
			break;
	}

	bulPosition.Zero();
	if (Ragebot::data.player->GetVelocity().Length() < 20.f && localplayer->GetVelocity().Length() < 20.f){
		engine->ExecuteClientCmd("say I don't Shoot Gays");
		Resolver::players[Ragebot::data.player->GetIndex()].MissedCount++;
	}else {
		engine->ExecuteClientCmd("say Shit this spread sucking so hard");
	}
	
}


void Ragebot::init(C_BasePlayer* _localplayer, C_BaseCombatWeapon* _activeWeapon)
{
	localplayer = _localplayer;
	activeWeapon = _activeWeapon;
	enemy = nullptr;
}

/* END */

void RagebotNoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!(cmd->buttons & IN_ATTACK) 
		|| !localplayer
		|| !localplayer->GetAlive())
		return;
	
	// if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_SSG08 || *activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_AWP)
	// 	return;

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

void RagebotAutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings)
{
    if (!localplayer || !localplayer->GetAlive() || !currentSettings.AutoCroutch)
		return;
	
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    cmd->buttons |= IN_DUCK | IN_BULLRUSH;
}

void RagebotAutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle, const RageWeapon_t& currentSettings)
{
	if (!currentSettings.autoSlow)
		return;
	if (!localplayer || !localplayer->GetAlive())
		return;
	if ( !activeWeapon || activeWeapon->GetInReload())
		return;
	
	if (currentSettings.autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK)){ 
		cmd->buttons |= IN_ATTACK2; 
	}
		
	Ragebot::data.needToStop = true;
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime ) {
		return;
	}
		

	
	QAngle ViewAngle;
		engine->GetViewAngles(ViewAngle);
	static Vector oldOrigin = localplayer->GetAbsOrigin( );
	Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin ) * (1.f/globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin( );
	float speed  = velocity.Length( );
		
	// if(speed > 30.f)
	// {
	// 	QAngle dir;
	// 	Math::VectorAngles(velocity, dir);
	// 	dir.y = ViewAngle.y - dir.x;
		
	// 	Vector NewMove = Vector(0);
	// 	Math::AngleVectors(dir, NewMove);
	// 	auto max = std::max(forrwordMove, sideMove);
	// 	auto mult = 450.f/max;
	// 	NewMove *= -mult;
			
	// 	cmd->forwardmove = NewMove.x;
	// 	cmd->sidemove = NewMove.y;
	// }
	// else 
	// {
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
	// }
	// CreateMove::sendPacket = false;
}

void RagebotAutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
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
			if ( !Ragebot::canShoot(activeWeapon, bestspot, player, currentSettings))
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

void RagebotAutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings)
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
		
	if ( Ragebot::canShoot(activeWeapon, bestspot, player, currentSettings) )
	{
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;
		return;
	}

	
	RagebotAutoSlow(localplayer, activeWeapon, cmd, forrwordMove, sideMove, angle, currentSettings);
}

inline void AutoPistol(C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, const RageWeapon_t& currentSettings)
{
	if (!activeWeapon || activeWeapon->GetInReload())
		return;
	if (!currentSettings.autoPistolEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetCSWpnData()->GetWeaponType() != CSWeaponType::WEAPONTYPE_PISTOL)
		return;
	if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
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
	C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
		return;
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
		return;

	init(localplayer, activeWeapon);
	CheckHit();
	
	if (!localplayer->GetAlive() || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime)
		return;

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;
	
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

	if (data.player){
		data.player = nullptr;
		data.shooted = false;
		data.PrevTickEyePosition.Zero();
	}
	data.needToStop = false;

	localEye = localplayer->GetEyePosition();
   	BestSpot.Zero();
	BestDamage = 0;
		
	Ragebot::GetBestEnemy();
	
    if (enemy && Ragebot::BestDamage > 0)
    {
		Settings::Debug::AutoAim::target = Ragebot::BestSpot;

		RagebotAutoShoot(enemy, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, *currentWeaponSetting);
    	RagebotAutoR8(enemy, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, oldForward, oldSideMove, *currentWeaponSetting);
		RagebotAutoCrouch(enemy, cmd, activeWeapon, *currentWeaponSetting);

		if (cmd->buttons & IN_ATTACK)
		{
			Ragebot::data.player = enemy;
			Ragebot::data.playerhelth = enemy->GetHealth();
			Ragebot::data.PrevTickEyePosition = Ragebot::localEye;
			Ragebot::data.shooted = true;
			if (Settings::AntiAim::InvertOnShoot) { Settings::AntiAim::inverted = !Settings::AntiAim::inverted; }
			Math::CalcAngle(Ragebot::localEye, Ragebot::BestSpot, angle);
		}
    }
	
	RagebotNoRecoil(angle, cmd, localplayer, activeWeapon, *currentWeaponSetting);
	AutoPistol(activeWeapon, cmd, *currentWeaponSetting);

    Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    FixMouseDeltas(cmd, enemy, angle, oldAngle);
    cmd->viewangles = angle;

	if (!currentWeaponSetting->silent)
		engine->SetViewAngles(angle);

}

void Ragebot::FireGameEvent(IGameEvent* event)
{
	if(!event)	
		return;

	if ( strcmp(event->GetName(), XORSTR("bullet_impact")) == 0 && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) == engine->GetLocalPlayer()){
		const float x = event->GetFloat(XORSTR("x")), 
					y = event->GetFloat(XORSTR("y")), 
					z = event->GetFloat(XORSTR("z"));
		bulPosition.Init(x, y, z);
		// cvar->ConsoleDPrintf(XORSTR("Bullet Hit : %0.0f, %0.0f, %0.0f\n"), x, y, z);
	}
    else if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    	return;
    }
    else if (strcmp(event->GetName(), XORSTR("player_death")) == 0)
    {
		// cvar->ConsoleColorPrintf(ColorRGBA(255,200,156,255), XORSTR("player_death\n"));
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
	    	return;

		if (attacker_id != engine->GetLocalPlayer())
	    	return;
    }
}
