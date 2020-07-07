#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../Hacks/legitbot.h"
#include "../Hacks/autowall.h"
#include "../Hacks/autowall.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

#ifndef LessThan
    #define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
    #define IsEqual(x, y) (x == x)
#endif

QAngle AntiAim::LastTickViewAngle;
static bool needToFlick = false;


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

		Vector cbVecTarget = player->GetAbsOrigin();
		
		float cbFov = Math::GetFov( viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget) );
		
		if (prevFOV == 0.f)
		{
			prevFOV = cbFov;
			closestPlayer = player;
		}
		else if ( cbFov < prevFOV )
		{
			return player;
		}
		else 
			break;
	}
	return closestPlayer;
}

static float GetBestHeadAngle(CUserCmd* cmd)
{    
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	
    if (!localplayer)
		return 0;
    
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
        return 0;

    C_BasePlayer* target = GetClosestEnemy(cmd);

    if (!target)
        return 0;
    if (!target->GetAlive())
        return 0;
	
	// we will require these eye positions
	Vector eye_position = localplayer->GetEyePosition();
	// lets set up some basic values we need
	int best_damage = localplayer->GetHealth();
	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
    float angle_step = 45.f;
	// our result
	float yaw = 0.0f;

    // iterate through 45.0f deg angles  
	for( float n = 0.0f; n < 360.f; n += angle_step )
	{
		// shoutout to aimtux for headpos calc
		Vector head_position( cos( n ) + eye_position.x,
							sin( n ) + eye_position.y,
							eye_position.z);

        AutoWall::FireBulletData data;
        float damage = AutoWall::GetDamage(head_position, target, true, data);
		if( damage < best_damage && damage > 0.f)
		{
			best_damage = data.current_damage;
			yaw = n;
		}
	}
   
	return yaw;
}

static void LBYBreak(float offset, QAngle& angle,C_BasePlayer* localplayer)
{
    static bool lbyBreak = false;
    static float lastCheck = 0.f;
    float vel2D = localplayer->GetVelocity().Length2D();
    if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        } 
        else {
            if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                AntiAim::realAngle.y = angle.y -= offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                AntiAim::realAngle.y = angle.y -= offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
}

// Funtion for Rage Anti AIm
static void DefaultRageAntiAim(C_BasePlayer *const localplayer,QAngle& angle, QAngle& ViewAngle, CUserCmd* cmd)
{
    
    using namespace Settings::AntiAim::RageAntiAim;
    using namespace AntiAim::ManualAntiAim;


    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (alignLeft)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y-90.f);
            return;
        }
        else if (alignBack)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y-180.f);
            return;
        }
        else if (alignRight)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y+90.f);
            return;
        }
    }

    const float &maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (Settings::AntiAim::HeadEdge::enabled)    {
        const float &BestHeadAngle =  GetBestHeadAngle(cmd);
        if (BestHeadAngle){
            if (AntiAim::bSend){
                static bool invert = false;
                invert = !invert;
                AntiAim::realAngle.y = *localplayer->GetLowerBodyYawTarget() = invert ? BestHeadAngle - maxDelta : BestHeadAngle + maxDelta;
            }else { // Not Sending Actual Body
                AntiAim::fakeAngle.y = *localplayer->GetLowerBodyYawTarget() = BestHeadAngle;
            }
            return;
        }
    }
    
    bool buttonToggle = false;

    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;


    static bool bFlip = false;
    if (CreateMove::sendPacket)
        bFlip = !bFlip;
    
    if(AntiAim::bSend)
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;
            case AntiAimFakeType_y::Static:
                *localplayer->GetLowerBodyYawTarget() = AntiAim::fakeAngle.y = angle.y = inverted ? (ViewAngle.y-180.f)+GetPercentVal(maxDelta, AntiAImPercent) : (ViewAngle.y-180.f)-GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Jitter:
                 *localplayer->GetLowerBodyYawTarget() = AntiAim::fakeAngle.y = angle.y = bFlip ?  AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Randome:
                 *localplayer->GetLowerBodyYawTarget() = AntiAim::fakeAngle.y = angle.y = ViewAngle.y - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/360.f);
                break;
        }   
    }     
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;
            case AntiAimRealType_Y::Static:
                //  *localplayer->GetLowerBodyYawTarget() = 
                 AntiAim::realAngle.y = angle.y = inverted ? (ViewAngle.y-180.f)-GetPercentVal(maxDelta, AntiAImPercent) : (ViewAngle.y-180.f)+GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimRealType_Y::Jitter:
                //  *localplayer->GetLowerBodyYawTarget() = 
                 AntiAim::realAngle.y = angle.y = bFlip ? (ViewAngle.y-180.f) + GetPercentVal(360.f, JitterPercent) : (ViewAngle.y-180.f) - GetPercentVal(360.f, JitterPercent);
                break;
            case AntiAimRealType_Y::Randome: 
                //  *localplayer->GetLowerBodyYawTarget() = 
                 AntiAim::realAngle.y = angle.y = ViewAngle.y - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/360.f));
                    break;
        }
        inverted ? LBYBreak(*localplayer->GetLowerBodyYawTarget()-GetPercentVal(maxDelta, AntiAImPercent), angle, localplayer) : LBYBreak(*localplayer->GetLowerBodyYawTarget()+GetPercentVal(maxDelta, AntiAImPercent), angle, localplayer);
        // LBYBreak(angle.y + 45.f, angle, localplayer);
    }

}

static void FreeStand(C_BasePlayer *const localplayer, QAngle& angle,QAngle& ViewAngle, CUserCmd* cmd){

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;
    
    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (alignLeft)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y - 90.f);
            return;
        }
        else if (alignBack)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y-180.f);
            return;
        }
        else if (alignRight)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y = (ViewAngle.y+90.f);
            return;
        }
    }

    if (Settings::AntiAim::HeadEdge::enabled)    {
        const float &BestHeadAngle =  GetBestHeadAngle(cmd);
        if (BestHeadAngle){
                AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = BestHeadAngle;
            return;
        }
    }

    static bool buttonToggle = false;
    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;

    switch (Settings::AntiAim::Yaw::typeReal)
    {
    case AntiAimRealType_Y::Jitter:
        static bool bFlip = false;
        angle.y = bFlip ? (ViewAngle.y-180.f) - GetPercentVal(360.f, JitterPercent) : (ViewAngle.y-180.f) + GetPercentVal(360.f, JitterPercent);
        bFlip = !bFlip;
        break;
    case AntiAimRealType_Y::Static:
        angle.y = (ViewAngle.y-180.f);
        break;
    case AntiAimRealType_Y::NONE:
        break;
    default:
        break;
    }
    AntiAim::fakeAngle= AntiAim::realAngle= angle;
    
}

// Function to set the pitch angle
static void DoAntiAimX(QAngle& angle)   
{ 
        AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 89.f;
}

// Function For Legit AntiAim
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool& bSend, CUserCmd* cmd)
{
    if (!localplayer->GetAlive() || !localplayer)
        return;

    using namespace Settings::AntiAim::LegitAntiAim;

    static bool buttonToggle = false;
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    const float &maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    QAngle ViewAngle;
        engine->GetViewAngles(ViewAngle);

    if (inverted){
        if (AntiAim::bSend) {
            AntiAim::fakeAngle.y = angle.y = ViewAngle.y;
        }else {
            // AntiAim::realAngle.y = 
            AntiAim::realAngle.y = angle.y = ViewAngle.y-GetPercentVal(maxDelta, RealPercentage);
            LBYBreak(ViewAngle.y+GetPercentVal(maxDelta, RealPercentage), angle, localplayer);
        }
    }else {
        if (AntiAim::bSend) {
            // *localplayer->GetLowerBodyYawTarget() = ViewAngle.y-GetPercentVal(maxDelta, RealPercentage);
            AntiAim::fakeAngle.y = angle.y = ViewAngle.y;
        }else {
            // AntiAim::realAngle.y = 
            AntiAim::realAngle.y = angle.y = ViewAngle.y+GetPercentVal(maxDelta, RealPercentage);
            LBYBreak(ViewAngle.y+GetPercentVal(maxDelta, RealPercentage), angle, localplayer);
        }
    }
}

/*
 * Settings Manual Anti AIm 
 * Simply What we do is change the value of YAxis for in Settings::AntiAIm::Yaw::real
 * Basically THis si work as toggle
 */
static bool DoManuaAntiAim(C_BasePlayer* localplayer, QAngle& angle)
{
    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;

    static bool Bpressed = false;
    
    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return false;

    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		alignBack = !alignBack;
		alignLeft = alignRight = false;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		alignRight = !alignRight;
        alignBack = alignLeft = false;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
    {
		alignBack = alignRight = false;
        alignLeft = !alignLeft;
	}
    bool buttonNotPressed = !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton );	
    
    if (buttonNotPressed && Bpressed)
        Bpressed = false;
}

// This Method have all the condition where AntiAim Should not work enjoy :)
static bool canMove(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeweapon, CUserCmd* cmd)
{
    
    if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
        return false;
    
    if (activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeweapon;
        if (csGrenade->GetThrowTime() > 0.f)
            return false;
    }
    if (cmd->buttons & IN_ATTACK)
    {
        CreateMove::sendPacket = true;
        return false;
    }
    if (*activeweapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
    {
        const float& postponTime = activeweapon->GetPostPoneReadyTime();
        if (cmd->buttons & IN_ATTACK2)
            return false;
        if (postponTime <= globalVars->curtime )
            return true;
    }
    if ( cmd->buttons & IN_USE )
    {
        static bool bFlip = true;
        bFlip = !bFlip;
        bFlip ? *localplayer->GetLowerBodyYawTarget() = cmd->viewangles.y - AntiAim::GetMaxDelta(localplayer->GetAnimState()) : *localplayer->GetLowerBodyYawTarget() = cmd->viewangles.y + AntiAim::GetMaxDelta(localplayer->GetAnimState());
        return false;
    }

    if ( cmd->buttons & IN_ATTACK2 && activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE )
        return false;
    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return false;

    return true;
}

void AntiAim::CreateMove(CUserCmd* cmd)
{
    if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enable)
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
        return;
    
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

    if (!activeWeapon)
        return;

    if ( !canMove(localplayer, activeWeapon, cmd) )
    {
        AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }

    if (Settings::FakeLag::enabled)
        AntiAim::bSend = CreateMove::sendPacket;        
    else 
        AntiAim::bSend = CreateMove::sendPacket = !CreateMove::sendPacket;

    QAngle angle = cmd->viewangles;
    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    needToFlick = false;

    if( Settings::AntiAim::LBYBreaker::enabled ){
        LBYBreak(Settings::AntiAim::LBYBreaker::offset, angle, localplayer);
    }
    if (Settings::AntiAim::RageAntiAim::enable && !needToFlick) // responsible for reage anti aim or varity of anti aims .. 
    {
        QAngle ViewAngle;
        engine->GetViewAngles(ViewAngle);

        DoManuaAntiAim(localplayer, angle);
        if (Settings::AntiAim::RageAntiAim::atTheTarget)
        {
            C_BasePlayer* lockedTarget = GetClosestEnemy(cmd);
            if (lockedTarget)
                ViewAngle = Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition());
        }
    
        switch (Settings::AntiAim::RageAntiAim::Type)
        {
            case RageAntiAimType::DefaultRage:
                DefaultRageAntiAim(localplayer, angle, ViewAngle, cmd );
                break;
            case RageAntiAimType::FreeStand:
                FreeStand(localplayer, angle, ViewAngle, cmd);
                break;
        }       
            
        DoAntiAimX(angle); // changing the x View Angle
    }
    else if (Settings::AntiAim::LegitAntiAim::enable && !needToFlick) // Responsible for legit anti aim activated when the legit anti aim is enabled
        DoLegitAntiAim(localplayer, angle, AntiAim::bSend, cmd);

    CreateMove::lastTickViewAngles = angle;
    Math::NormalizeAngles(angle);
    cmd->viewangles = angle;
    // Math::ClampAngles(angle);
    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);    
}
