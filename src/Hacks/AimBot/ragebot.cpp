// #pragma GCC diagnostic ignored "-Wcomment"
// #pragma GCC diagnostic ignored "-Warray-bounds"

#include <cstdlib>
#include <ctime>

#include "ragebot.hpp"
#include "resolver.h"
// #include "../../hooker.cpp"
#include "../AntiAim/fakelag.h"
#include "../esp.h"
#include "aimbot.hpp"


QAngle RCSLastPunch;

void Ragebot::BestHeadPoint(C_BasePlayer* player, const int &BoneIndex,int& Damage, Vector& Spot)
{
	model_t *pModel = player->GetModel();
	if( !pModel )
		return;

	studiohdr_t *hdr = modelInfo->GetStudioModel(pModel);
	if( !hdr )
		return;

	mstudiobbox_t *bbox = hdr->pHitbox(BoneIndex, 0);
	if( !bbox )
		return;

	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, Ragebot::BoneMatrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, Ragebot::BoneMatrix[bbox->bone], maxs);

	Vector center = ( mins + maxs ) * 0.5f;
	Vector points[11] ={ 
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

	for (int i = 0; i < 11; i++)
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

void Ragebot::BestMultiPoint(C_BasePlayer* player,const int &BoneIndex,int& Damage, Vector& Spot)
{
	model_t* pModel = player->GetModel();
    if (!pModel)
		return;
    studiohdr_t* hdr = modelInfo->GetStudioModel(pModel);
    if (!hdr)
		 return;
	mstudiobbox_t* bbox = hdr->pHitbox(BoneIndex, 0);
	if (!bbox)
		return;
	
	Vector mins, maxs;
	Math::VectorTransform(bbox->bbmin, Ragebot::BoneMatrix[bbox->bone], mins);
	Math::VectorTransform(bbox->bbmax, Ragebot::BoneMatrix[bbox->bone], maxs);
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
	if (!enemy || !enemy->IsAlive())
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
		
		if (!enemy || !enemy->IsAlive())
			return false;

		srand((unsigned int)time(NULL));
		
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

		static Vector viewForward;
		Math::AngleVectors(viewAnglesSpread, viewForward);
		viewForward.NormalizeInPlace();

		viewForward = src + (viewForward * activeWeapon->GetCSWpnData()->GetRange());

        trace_t tr;
        Ray_t ray;

       	ray.Init(src, viewForward);
        trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, enemy, &tr);

        if (tr.m_pEntityHit == enemy)
           	hitCount++;

        if (static_cast<int>((hitCount*0.390625)) >= currentSettings.HitChance) // 1/(100/256 = 0.390625 && 
			return true;

		if ((256 - i + hitCount) < NeededHits)
			return false;
    }

    return false;
}

void Ragebot::GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, const int& playerhealth,int& i,const std::unordered_map<int, int>* modelType)
{	

	if (!player || !player->IsAlive())
		return;

	static auto HitboxHead([&](int &BoneID){
		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		BestHeadPoint(player, BoneID, Damage, Spot);
	});
	static auto UpperSpine([&](int &BoneID){

		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);

		if (Damage >= playerhealth)
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
			
			if (bestDamage >= playerhealth)
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
		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= playerhealth)
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
			Vector bone3D = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
			
			int bestDamage = AutoWall::GetDamage(bone3D, true);
			if (bestDamage >= playerhealth)
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

		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		// BestMultiPointDamage(player, BoneID, Damage, Spot);
		if (Damage >= playerhealth)
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
			Vector bone3D = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
			int bestDamage = AutoWall::GetDamage(bone3D, true);

			if (bestDamage >= playerhealth)
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
		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);

		if (Damage >= playerhealth)
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
			Vector bone3D = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
			int bestDamage = AutoWall::GetDamage(bone3D, true);

			if (bestDamage >= playerhealth)
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

		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
		Damage = AutoWall::GetDamage(Spot, true);
		BestMultiPoint(player, BoneID, Damage, Spot);
		if (Damage >= playerhealth)
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
			Vector bone3D = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);		
			int bestDamage = AutoWall::GetDamage(bone3D, true);

			if (bestDamage >= playerhealth)
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
		Spot = player->GetBonePosition(BoneID, Ragebot::BoneMatrix);
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
	if (!localplayer || !localplayer->IsAlive())
		return;

	static Vector bestSpot;
	static int bestDamage;
	bestSpot.Zero();
	bestDamage = 0;

	int maxClient = engine->GetMaxClients();
	C_BasePlayer* player = nullptr;
	static int boneIndex = 0;
	for (int i = 1; i < maxClient; ++i)
	{
		player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			i == engine->GetLocalPlayer() || 
			player->GetDormant() || 
			!player->IsAlive() || 
			player->GetImmune() || 
			player->GetTeam() == localplayer->GetTeam() )
			continue;			

		if ( !player->SetupBones(Ragebot::BoneMatrix, 128, 256, 0))
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
			
		int playerhealth = player->GetHealth();
		const std::unordered_map<int, int>* modelType = BoneMaps::GetModelTypeBoneMap(player);

		for (boneIndex = 0; boneIndex < 6; boneIndex++)
		{
			if ( !currentWeaponSetting->desireBones[boneIndex] ) continue;

			// if (boneIndex == 0 && player->GetHealth() < 70)
			// 	boneIndex++;

			GetDamageAndSpots(player, bestSpot, bestDamage, playerhealth, boneIndex, modelType);

			if (bestDamage >= playerhealth)
			{
				BestDamage = bestDamage;
				BestSpot = bestSpot;
				enemy = player;
				return;
			}
			else if ( inputSystem->IsButtonDown(Settings::Ragebot::DamageOverrideBtn) ){
				if (bestDamage >= currentWeaponSetting->DamageOverride && bestDamage > BestDamage ){
					BestDamage = bestDamage;
					BestSpot = bestSpot;
					enemy = player;
				}
				
			}
			else if (bestDamage >= currentWeaponSetting->MinDamage && bestDamage > BestDamage)
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
			Ragebot::data.Hitted = true;
			// if ( fireBulletData.enter_trace.m_pEntityHit->GetIndex() == Ragebot::data.player->GetIndex())
			// 	cvar->ConsoleColorPrintf(ColorRGBA(254, 0, 0, 255), XORSTR("Hit Our Player I guess\n"));
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

void Ragebot::AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, RageWeapon_t* currentSettings)
{
    if (!currentSettings->autoShootEnabled)
		return;
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;
    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE)
		return;
	
	if (currentSettings->autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) && !localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons&IN_ATTACK)){ 
		cmd->buttons |= IN_ATTACK2; 
	}

	bool canShoot;
	canShoot = false;
	if (currentSettings->hitchanceType == HitchanceType::Normal){
		canShoot = Aimbot::canShoot(localplayer, activeWeapon, currentSettings->HitChance);
	}else {
		canShoot = Ragebot::canShoot(activeWeapon, bestspot, player, *currentSettings);
	}
	if ( canShoot )
	{
		if (activeWeapon->GetNextPrimaryAttack() >= globalVars->curtime)
			cmd->buttons &= ~IN_ATTACK;
		else if ( !(cmd->buttons & IN_ATTACK) )
			cmd->buttons |= IN_ATTACK;

		Ragebot::data.autoslow = false;

		return;
	}

	// static prevSpe
	Aimbot::AutoSlow(localplayer, cmd, currentSettings->autoSlow);
}

void Ragebot::CreateMove(CUserCmd* cmd)
{
	if (Ragebot::data.player && Ragebot::data.player->IsAlive() )
    {
		if (Ragebot::data.Hitted && Ragebot::data.player->GetHealth() >= Ragebot::data.playerhealth){
            cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("Miss Due To Resolver\n"));
			engine->ExecuteClientCmd("say Resolver is dog shit");
		    Resolver::players[Ragebot::data.player->GetIndex()].MissedCount++;
        }
		else if (!Ragebot::data.Hitted){
			engine->ExecuteClientCmd("say AimBot Is Dog Shit");
		}
    }

	data.player = nullptr;
	Ragebot::data.Hitted = false;

	localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->IsAlive())
		return;
    activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0)
		return;

	

	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime) // fix of shit granade bug cause by me :P
		return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
    if (Settings::Ragebot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) != Settings::Ragebot::weapons.end()){
		index = *activeWeapon->GetItemDefinitionIndex();
	}
	currentWeaponSetting = &Settings::Ragebot::weapons.at(index);

	static QAngle oldAngle = QAngle(0);
	static QAngle angle = QAngle(0);
	
	angle = cmd->viewangles;
    engine->GetViewAngles(oldAngle);

    CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
    if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE || weaponType == CSWeaponType::WEAPONTYPE_KNIFE)
		return;

	Ragebot::localEye = localplayer->GetEyePosition();
   	Ragebot::BestSpot.Zero();
	Ragebot::BestDamage = (int)0;
	ShootEnemyIndex = 0;
	enemy = nullptr;

	Ragebot::GetBestEnemy();
	
    if (enemy && Ragebot::BestDamage > 0)
    {
		Settings::Debug::AutoAim::target = Ragebot::BestSpot;

		AutoShoot(enemy, localplayer, activeWeapon, cmd, Ragebot::BestSpot, angle, currentWeaponSetting);
		Aimbot::AutoCrouch(cmd, activeWeapon, currentWeaponSetting->AutoCrouch);

		if (cmd->buttons & IN_ATTACK)
		{
			Ragebot::data.player = enemy;
			Ragebot::data.playerhealth = enemy->GetHealth();
			Ragebot::data.PrevTickEyePosition = Ragebot::localEye;
			Ragebot::data.shooted = true;
			if (Settings::AntiAim::InvertOnShoot) { Settings::AntiAim::inverted = !Settings::AntiAim::inverted; }
			Math::CalcAngle(Ragebot::localEye, Ragebot::BestSpot, angle);
			cmd->sidemove = cmd->forwardmove = 0;
		}
    }
	
	Aimbot::AutoR8(enemy, activeWeapon, cmd, currentWeaponSetting->autoShootEnabled);
	Aimbot::NoRecoil(angle, cmd, localplayer, activeWeapon, currentWeaponSetting->silent);
	Aimbot::AutoPistol(activeWeapon, cmd, currentWeaponSetting->autoPistolEnabled);

	Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

    Aimbot::FixMouseDeltas(cmd, angle, oldAngle);

    cmd->viewangles = angle;

	if (cmd->buttons & IN_ATTACK){
		CreateMove::sendPacket = true;
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
