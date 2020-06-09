#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../Hacks/legitbot.h"
#include "../Hacks/autowall.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "ragebot.h"

#ifndef GetPercentVal
    #define GetPercentVal(val, percent) ( val * (percent/100) )
#endif

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

static bool GetBestHeadAngle(QAngle &angle, bool bSend, CUserCmd* cmd)
{
    if (!Settings::AntiAim::HeadEdge::enabled)
        return false;
    
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return false;

    C_BasePlayer* target = GetClosestEnemy(cmd);

    if (!target)
        return false;

	float b, r, l;
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
	Vector src3D, dst3D, forward, right, up;

	trace_t tr;
	Ray_t ray, ray2, ray3;
	CTraceFilter filter;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, &forward, &right, &up);

	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);

	ray.Init(src3D, dst3D);
	trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	b = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	r = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	l = (tr.endpos - tr.startpos).Length();

	// if (b < r && b < l && l == r) // Left = Right > Back
	// 	return true;

	if (b > r && b > l) // Back
		angle.y = viewAngles.y + bSend ?  180 + maxDelta : 180;
	else if (r > l && r > b) // Right
	{
		angle.y = viewAngles.y + bSend ? 90 + maxDelta : 90;
		// lbyBreak = 90 + maxDelta;
	}
	else if (r > l && r == b) // Right = Back
	{
		angle.y = viewAngles.y + bSend ? 135 + maxDelta : 135;
		// lbyBreak = 135 + maxDelta;
	}
	else if (l > r && l > b) // Left
	{
		angle.y = viewAngles.y - bSend ? 90 + maxDelta : 90;
		// lbyBreak = 90 - maxDelta;
	}
	else if (l > r && l == b) // Left = Back
	{
		angle.y = viewAngles.y - bSend ? 135 + maxDelta : 135;
		// lbyBreak = 135 - maxDelta;
	}

	return true;
}

// Funtion for Rage Anti AIm
static void FakeArroundRealAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool bSend)
{
    
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
   
    bool buttonToggle = false;

    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);
    // cvar->ConsoleDPrintf("in real arround fake");
        
    if( bSend )
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::Static:
                angle.y = inverted ? AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Jitter:
                static bool bFlip = false;
                bFlip = !bFlip;
                angle.y = bFlip ?  AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    } 
            
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::Static :
                angle.y -= 180.f;
                break;
            case AntiAimRealType_Y::Jitter:
                static bool bFlip = false;
                bFlip = !bFlip;
                angle.y -= bFlip ? 180.f + GetPercentVal(180.f, JitterPercent) : 180.f - GetPercentVal(180.f, JitterPercent);
                break;
            default:
                break;
        }
        AntiAim::realAngle.y = angle.y;
    }

}

static void RealArroundFakeAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool bSend)
{
    
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
   
    static bool buttonToggle = false;
    /* Button Function for invert the fake*/
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    
    QAngle ViewAngle;
    engine->GetViewAngles(ViewAngle);
    // cvar->ConsoleDPrintf("in fake arround real\n");
    if( bSend )
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::Static :
                angle.y = ViewAngle.y - 180.f;
                break;
            case AntiAimFakeType_y::Jitter :
                static bool bFlip = false;
                bFlip = !bFlip;
                angle.y = bFlip ? ViewAngle.y - 180.f - GetPercentVal(180.f, JitterPercent) : ViewAngle.y - 180.f + GetPercentVal(180.f, JitterPercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    } 
            
    else
    {            
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::Static:
                angle.y = inverted ? AntiAim::fakeAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::fakeAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimRealType_Y::Jitter:
                static bool bFlip = false;
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::fakeAngle.y + GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::fakeAngle.y - GetPercentVal(maxDelta, AntiAImPercent);
                break;
            default:
                break;
        }
            
        AntiAim::realAngle.y = angle.y;
    }        
}

// Function to set the pitch angle
static void DoAntiAimX(QAngle& angle)
{
    if (!ValveDSCheck::forceUT && ((*csGameRules) && (*csGameRules)->IsValveDS()))
            AntiAim::realAngle.x = AntiAim::fakeAngle.x = angle.x = 89.f;
    else 
            AntiAim::realAngle.x = AntiAim::fakeAngle.x = angle.x = 36000088.0f;
}

// Function For Legit AntiAim
static void DoLegitAntiAim(C_BasePlayer *const localplayer, QAngle& angle, bool& bSend, CUserCmd* cmd)
{
    using namespace Settings::AntiAim::LegitAntiAim;
    static bool buttonToggle = false;
    if ( inputSystem->IsButtonDown(InvertKey) && !buttonToggle )
	{
		buttonToggle = true;
		inverted = !inverted;
	}
	else if ( !inputSystem->IsButtonDown(InvertKey) && buttonToggle)
		buttonToggle = false;
    
    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

    QAngle ViewAngle;
        engine->GetViewAngles(ViewAngle);
    /*
    * Actual area where we are settings the fake and real angle 
    * area is the variable which is responcible for changing angle so area is the vital variable
    * AntiAim::realangle and AntiAim::fakeangle is are set to determine the angle in thirdperson mode
    */
    if(!inverted)
    {
        if (AntiAim::bSend)
        {
            AntiAim::fakeAngle = angle;        
        }
        else
        {
            if ( cmd->buttons & IN_DUCK)
                angle.y += GetPercentVal(maxDelta, RealPercentageInCroutch);
            else 
                angle.y += GetPercentVal(maxDelta, RealPercentage);
            
            AntiAim::realAngle = angle;
        }
        
    }
    
    else 
    {
        if (AntiAim::bSend )
        {
            AntiAim::fakeAngle = angle;
        }
        else 
        {
            if ( cmd->buttons & IN_DUCK )
                angle.y -= GetPercentVal(maxDelta, RealPercentageInCroutch);
            else 
                angle.y -= GetPercentVal(maxDelta, RealPercentage);
            
            AntiAim::realAngle = angle;
        } 
    } 
        
}


/*
** Settings Manual Anti AIm 
** Simply What we do is change the value of YAxis for in Settings::AntiAIm::Yaw::real
** Basically THis si work as toggle
*/
static bool DoManuaAntiAim(C_BasePlayer* localplayer, const bool& bSend, QAngle& angle)
{

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    static bool bFlip = false;

    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return false;
    
    // Manual anti aim set to back
    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		alignBack = !alignBack;
		alignRight = false;
        alignLeft = false;
	}	

    //END BACK MANUAL

    //SETTING MANUAL FOR RIGHT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		alignBack = false;
		alignRight = !alignRight;
        alignLeft = false;
	}	

    //END MANUAL RIGHT

    // SETTINGS MANUAL ANTIAIM FOR LEFT
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
	{
		alignBack = false;
		alignRight = false;
        alignLeft = !alignLeft;
	}
    bool buttonNotPressed = !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton );	
    
    if (buttonNotPressed && Bpressed)
        Bpressed = false;

    AntiAimFakeType_y Fake_aa_type = Settings::AntiAim::Yaw::typeFake;
    // Applying ManualAntiAIm
    if ( !AntiAim::bSend )
    {
        if ( alignBack )
            angle.y += 180.f;
        else if (alignLeft)
            angle.y += 90.f;
        else if (alignRight)
            angle.y -= 90.f;

        AntiAim::realAngle.y = angle.y;
    }
    else
    {
        switch (Fake_aa_type)
        {
            case AntiAimFakeType_y::Static:
                angle.y = inverted ? AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            case AntiAimFakeType_y::Jitter:
                bFlip = !bFlip;
                angle.y = bFlip ? AntiAim::realAngle.y - GetPercentVal(maxDelta, AntiAImPercent) : AntiAim::realAngle.y + GetPercentVal(maxDelta, AntiAImPercent);
                break;
            default:
                break;
        }
        AntiAim::fakeAngle.y = angle.y;
    }

    return (alignRight || alignLeft || alignBack);
}


void AntiAim::CreateMove(CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*) entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    
    if (!Settings::AntiAim::RageAntiAim::enable && !Settings::AntiAim::LBYBreaker::enabled && !Settings::AntiAim::LegitAntiAim::enable && !Settings::AntiAim::ManualAntiAim::Enable)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }    

    else if (Settings::Legitbot::AimStep::enabled && Legitbot::aimStepInProgress)
    {
        AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }

    else if (!localplayer || !localplayer->GetAlive())
        return;

    else if (activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE)
    {
        C_BaseCSGrenade* csGrenade = (C_BaseCSGrenade*) activeWeapon;

        if (csGrenade->GetThrowTime() > 0.f)
        {
            return;
        }
    }

    else if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER)
    {
        float postponTime = activeWeapon->GetPostPoneReadyTime();
        if (cmd->buttons & IN_ATTACK2)
            return;
            
        if ( postponTime < globalVars->curtime && postponTime > 0)
            return;
    }

    else if ( (cmd->buttons & IN_USE || cmd->buttons & IN_ATTACK) )
        return;
    
    else if ( cmd->buttons & IN_ATTACK2 && activeWeapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE )
        return;
    
    else if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;


    QAngle angle = cmd->viewangles;
    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    

    AntiAim::bSend = !AntiAim::bSend;
    bool needToFlick = false;
    static bool lbyBreak = false;
    static float lastCheck;
    float vel2D = localplayer->GetVelocity().Length2D();//localplayer->GetAnimState()->verticalVelocity + localplayer->GetAnimState()->horizontalVelocity;

    if( Settings::AntiAim::LBYBreaker::enabled ){

        if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
            lbyBreak = false;
            lastCheck = globalVars->curtime;
        } 
        else {
            if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                AntiAim::realAngle = angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                AntiAim::realAngle = angle.y -= Settings::AntiAim::LBYBreaker::offset;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
    }
    if (Settings::AntiAim::RageAntiAim::enable && !needToFlick) // responsible for reage anti aim or varity of anti aims .. 
    {
        // Check for manual antiaim
        if ( !DoManuaAntiAim(localplayer, AntiAim::bSend, angle)  )
        {
            
            //Look toword the target that aimbot locked on
            if (Settings::AntiAim::RageAntiAim::atTheTarget)
            {
                C_BasePlayer* lockedTarget = GetClosestEnemy(cmd);
                if (lockedTarget)
                    angle = Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition());
            }
                
            QAngle edge_angle = angle;
            bool edging_head = GetBestHeadAngle(edge_angle,bSend, cmd);
            
            if (!edging_head)
            {
                switch (Settings::AntiAim::RageAntiAim::Type)
                {
                    case RageAntiAimType::RealArroundFake:
                        //Settings::AntiAim::RageAntiAim::SendReal ? RealArroundFakeAntiAim(localplayer, angle, !AntiAim::bSend) : RealArroundFakeAntiAim(localplayer, angle, AntiAim::bSend);
                        RealArroundFakeAntiAim(localplayer, angle, AntiAim::bSend);
                        break;
                
                    case RageAntiAimType::FakeArroundReal:
                        // Settings::AntiAim::RageAntiAim::SendReal ? FakeArroundRealAntiAim(localplayer, angle, !AntiAim::bSend) : FakeArroundRealAntiAim(localplayer, angle, AntiAim::bSend);
                        FakeArroundRealAntiAim(localplayer, angle, AntiAim::bSend);
                        break;

                    default:
                        break;
                }       
            }
                else 
                {
                    if (!AntiAim::bSend)
                        angle.y = AntiAim::fakeAngle.y = edge_angle.y;
                    else
                        angle.y = AntiAim::realAngle.y = edge_angle.y;
                }
            }
            else 
            {
                // cvar->ConsoleDPrintf(XORSTR("edging head\n"));
                bSend ? AntiAim::realAngle.y = angle.y : AntiAim::fakeAngle.y = angle.y;
            }
            
        DoAntiAimX(angle);

        CreateMove::sendPacket = AntiAim::bSend;
    }
    else if (Settings::AntiAim::LegitAntiAim::enable && !needToFlick) // Responsible for legit anti aim activated when the legit anti aim is enabled
    {
        DoLegitAntiAim(localplayer, angle, AntiAim::bSend, cmd);
        CreateMove::sendPacket = AntiAim::bSend;
    }    
    
    Math::NormalizeAngles(angle);
    //Math::ClampAngles(angle);

    cmd->viewangles = angle;

    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);
}
