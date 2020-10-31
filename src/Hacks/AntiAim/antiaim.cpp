#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../AimBot/legitbot.h"
#include "../AimBot/autowall.h"
#include "../valvedscheck.h"
#include "../AimBot/ragebot.hpp"
#include "fakewalk.hpp"
#include "slowwalk.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))

#ifndef LessThan
    #define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
    #define IsEqual(x, y) (x == x)
#endif

QAngle AntiAim::LastTickViewAngle;

float AntiAim::GetMaxDelta( CCSGOAnimState *animState) 
{
    float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

    float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

    float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
    float unk2 = unk1 + 1.0f;
    float delta;

    if (animState->duckProgress > 0)
    {
        unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
    }

    delta = *(float*)((uintptr_t)animState + 0x3A4) * unk2;

    return delta - 0.5f;
}

static C_BasePlayer* GetClosestEnemy (CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return nullptr; 
	C_BasePlayer* closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
		engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

    int maxPlayers = engine->GetMaxClients();
	for (int i = 1; i < maxPlayers; ++i)
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

		Vector cbVecTarget = player->GetAbsOrigin();
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		
		if (prevFOV == 0.f)
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
		else if ( cbFov < prevFOV )
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
	}
	return closestPlayer;
}
/*
static float GetBestHeadAngle(CUserCmd* cmd)
{    
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	
    if (!localplayer || !localplayer->GetAlive())
		return 0;

    // C_BasePlayer* target = GetClosestEnemy(cmd);
    // we will require these eye positions
	Vector eye_position = localplayer->GetEyePosition();
	// lets set up some basic values we need
	int best_damage = localplayer->GetHealth();
	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
    static const float angle_step = 45.f;
	// our result
	float yaw = 0.0f;
    for (int i = 1; i <= engine->GetMaxClients() ; i++)
	{
        C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->GetAlive() || 
			player->GetImmune())
			continue;

        if (Entity::IsTeamMate(player, localplayer))
            return 0;
        // iterate through 45.0f deg angles  
	    for( float n = 0.0f; n < 180.f; n += angle_step )
	    {
	    	// shoutout to aimtux for headpos calc
	        Vector head_position( cos( n ) + eye_position.x,
						            sin( n ) + eye_position.y,
						            eye_position.z);

            AutoWall::FireBulletData data;
            int damage = AutoWall::GetDamage(player, head_position, true, data);
            // cvar->ConsoleDPrintf(XORSTR("AUTO DIRECTION DAMAGE : %d \n"), damage);
		    if( damage < best_damage && damage > -1)
		    {
		        best_damage = damage;
	            yaw = n;
		    }
	    }
    }
    return yaw;
	
}
*/

static bool GetBestHeadAngle(CUserCmd* cmd, QAngle& angle)
{
    if ( !Settings::AntiAim::autoDirection)
        return false;

    float b, r, l;

	Vector src3D, dst3D, forward, right, up, src, dst;

	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive( ))
		return false;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, &forward, &right, &up);

	auto GetTargetEntity = [ & ] ( void )
	{
		C_BasePlayer* bestTarget = nullptr;

        float prevFov = 0;
        const int &MaxClient = engine->GetMaxClients();
		for( int i = 1; i < MaxClient; i++ )
		{
			C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

			if (!player
				|| player == localplayer
				|| player->GetDormant()
				|| !player->GetAlive()
				|| player->GetImmune()
				|| player->GetTeam() == localplayer->GetTeam())
				continue;

			float fov = Math::GetFov(viewAngles, Math::CalcAngle(localplayer->GetEyePosition(), player->GetEyePosition()));

            if (prevFov == 0){
                bestTarget = player;
                prevFov = fov;
            }else if (fov > prevFov){
                bestTarget = player;
                prevFov = fov;
            }
		}

		return bestTarget;
	};

	C_BasePlayer* target = GetTargetEntity();
    if (!target)
        return false;
    // cvar->ConsoleDPrintf("Found Target\n");
	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);
    dst3D = target->GetBonePosition(BONE_HEAD);

	if (target == nullptr)
		return false;

	ray.Init(src3D, dst3D);
	trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	b = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	r = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	l = (tr.endpos - tr.startpos).Length();

	if (b < r && b < l && l == r)
		return false; //if left and right are equal and better than back

	if (b > r && b > l)
		AntiAim::realAngle.y = angle.y -= 180; //if back is the best angle
	else if (r > l && r > b)
		AntiAim::realAngle.y = angle.y += 90; //if right is the best angle
	else if (r > l && r == b)
		AntiAim::realAngle.y = angle.y += 135; //if right is equal to back
	else if (l > r && l > b)
		AntiAim::realAngle.y = angle.y -= 90; //if left is the best angle
	else if (l > r && l == b)
		AntiAim::realAngle.y = angle.y -= 135; //if left is equal to back
	else
		return false;

	return true;
}

static bool LBYBreak(float offset, QAngle& angle,C_BasePlayer* localplayer)
{
    if ( Settings::AntiAim::fakeAmmount == 0)
        return false;

    static bool lbyBreak;
    lbyBreak = false;
    static float lastCheck = 0.f;
    float vel2D = localplayer->GetVelocity().Length2D();
    if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
        lbyBreak = false;
        lastCheck = globalVars->curtime;
    } 
    else {
        if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
            if (Settings::AntiAim::inverted){
                angle.y += offset;
            }else {
                angle.y -= offset;
            }
            angle.x = CreateMove::lastTickViewAngles.x;
            lbyBreak = true;
            lastCheck = globalVars->curtime;
            AntiAim::fakeAngle = angle;
            AntiAim::bSend = false;
        } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        }
    }
    
    return lbyBreak;
}

static void DoManuaAntiAim()
{
    static bool Bpressed = false;
    
    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return;

    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		AntiAim::ManualAntiAim::alignBack = !AntiAim::ManualAntiAim::alignBack;
		AntiAim::ManualAntiAim::alignLeft = AntiAim::ManualAntiAim::alignRight = false;
        Bpressed = true;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		AntiAim::ManualAntiAim::alignRight = !AntiAim::ManualAntiAim::alignRight;
        AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignLeft = false;
        Bpressed = true;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
    {
		AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignRight = false;
        AntiAim::ManualAntiAim::alignLeft = !AntiAim::ManualAntiAim::alignLeft;
        Bpressed = true;
	}
    bool buttonNotPressed = !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton );	
    
    if (buttonNotPressed && Bpressed)
        Bpressed = false;    
}

static bool canMove(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeweapon, CUserCmd* cmd)
{
    ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeweapon->GetItemDefinitionIndex()) != Settings::Legitbot::weapons.end())
		index = *activeweapon->GetItemDefinitionIndex();
	const LegitWeapon_t& currentWeaponSetting = Settings::Legitbot::weapons.at(index);
    
    if (currentWeaponSetting.aimStepEnabled && Legitbot::aimStepInProgress)
        return false;
    
    if (activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeweapon;
        if (csGrenade->GetThrowTime() > 0.f)
            return false;
    }
    if (*activeweapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
    {
        const float& postponTime = activeweapon->GetPostPoneReadyTime();
        if (cmd->buttons & IN_ATTACK2)
            return false;
        if (postponTime < globalVars->curtime )
            return true;
    }
    if (cmd->buttons & IN_ATTACK || cmd->buttons & IN_USE)
        return false;
    if ( cmd->buttons & IN_ATTACK2 && activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE )
        return false;
    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return false;

    return true;
}

static void DoAntiAim(CUserCmd* cmd,C_BasePlayer* localplayer, QAngle& angle){

    if (GetBestHeadAngle(cmd, angle))
        return;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    
    // If the ammount if over 0 that means user wants to add fake angle else it is just the free stand
    static bool Bpressed = false;
    if ( inputSystem->IsButtonDown(Settings::AntiAim::InvertKey) && !Bpressed )
	{
		Settings::AntiAim::inverted = !Settings::AntiAim::inverted;
        Bpressed = true;
	}else if (!inputSystem->IsButtonDown(Settings::AntiAim::InvertKey) && Bpressed)
        Bpressed = false;

    if ( !AntiAim::bSend ){
        if (Settings::AntiAim::inverted) {
            angle.y -= Settings::AntiAim::fakeAmmount;
            AntiAim::realAngle.y = angle.y+28.f;
            AntiAim::realAngle.x = angle.x; 
            angle.y += maxDelta*2;          
        }
        else {
            angle.y += Settings::AntiAim::fakeAmmount;

            AntiAim::realAngle.y = angle.y-28.f;
            AntiAim::realAngle.x = angle.x;    
            angle.y -= maxDelta*2;
        }
    }
    else {
        if (Settings::AntiAim::inverted) {
            angle.y -= Settings::AntiAim::fakeAmmount;

            AntiAim::fakeAngle.y = angle.y-28.f;
            AntiAim::fakeAngle.x = angle.x;            
        }
        else {
            angle.y += Settings::AntiAim::fakeAmmount;

            AntiAim::fakeAngle.y = angle.y+28.f;
            AntiAim::fakeAngle.x = angle.x;
        }
    }

    // LBYBreak(CreateMove::lastTickViewAngles.y, angle, localplayer);
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if ( !Settings::AntiAim::Enabled)    
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    
    if (!activeWeapon) return;

    if ( !canMove(localplayer, activeWeapon, cmd) )
    {
        CreateMove::lastTickViewAngles = AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }
    
    if (Settings::FakeLag::enabled || FakeWalk::FakeWalking || SlowWalk::SlowWalking )
        AntiAim::bSend = CreateMove::sendPacket;             
    else
        CreateMove::sendPacket = AntiAim::bSend = !AntiAim::bSend;

    QAngle angle = cmd->viewangles;
    QAngle oldAngle;
     engine->GetViewAngles(oldAngle);
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    
    if (Settings::AntiAim::atTheTarget){
        C_BasePlayer* lockedTarget = GetClosestEnemy(cmd);
        if (lockedTarget)
            Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition(), angle);
    }
    if (Settings::AntiAim::PitchDown){
        angle.x = 89.f;
    }

    DoManuaAntiAim();
    if (AntiAim::ManualAntiAim::alignBack){
        angle.y += 180;
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
    }
    else if ( AntiAim::ManualAntiAim::alignRight ){
        angle.y -= 90;
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
    }else if ( AntiAim::ManualAntiAim::alignLeft){
        angle.y += 90;
        AntiAim::realAngle = AntiAim::fakeAngle = angle;
    }else {
        angle.y += Settings::AntiAim::offset;  
        DoAntiAim(cmd, localplayer, angle); 
    }

    Math::NormalizeAngles(angle);
    Math::ClampAngles(angle);

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);    
}

void AntiAim::FrameStageNotify(ClientFrameStage_t stage)
{
    // if (!Settings::AntiAim::Enabled)
    //     return;
    // if (stage != ClientFrameStage_t::FRAME_RENDER_START)
    //     return;
    // C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    // if (!localplayer || !localplayer->GetAlive())
    //     return;
}

void AntiAim::OverrideView(CViewSetup *pSetup)
{
    // // if (Settings::AntiAim::Type::antiaimType != AntiAimType::RageAntiAim && !Settings::AntiAim::LBYBreaker::enabled && Settings::AntiAim::Type::antiaimType == AntiAimType::LegitAntiAim)
    // //     return;

    // C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());

	// if (!localplayer || !localplayer->GetAlive( ))
	// 	return;

    // // pSetup->origin.x = localplayer->GetAbsOrigin().x + 64.0f;
}