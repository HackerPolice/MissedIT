#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../Hacks/AimBot/legitbot.h"
#include "../Hacks/AimBot/autowall.h"
#include "../../Utils/xorstring.h"
#include "../settings.h"
#include "../Hooks/hooks.h"
#include "../Utils/math.h"
#include "../Utils/entity.h"
#include "../interfaces.h"
#include "valvedscheck.h"
#include "AimBot/ragebot.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

#ifndef LessThan
    #define LessThan(x, y) (x < x)
#endif

#ifndef IsEqual
    #define IsEqual(x, y) (x == x)
#endif

QAngle AntiAim::LastTickViewAngle;
static bool needToFlick = false;

static float NormalizeAsYaw(float flAngle)
{
	if (flAngle > 180.f || flAngle < -180.f)
	{
		auto revolutions = round(abs(flAngle / 360.f));

		if (flAngle < 0.f)
			flAngle += 360.f * revolutions;
		else
			flAngle -= 360.f * revolutions;
	}

	return flAngle;
}

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

	for (int i = engine->GetMaxClients(); i > 1; i--)
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
	// cvar->ConsoleDPrintf(XORSTR("AUTO DIRECTION yaw : %f \n"), yaw);
    return yaw;
	
}

static bool GetBestHeadAngle(CUserCmd* cmd, QAngle& angle)
{
    float b, r, l;

	Vector src3D, dst3D, forward, right, up, src, dst;

	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer)
		return false;

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, &forward, &right, &up);

	auto GetTargetEntity = [ & ] ( void )
	{
		float bestFov = FLT_MAX;
		C_BasePlayer* bestTarget = NULL;

		for( int i = 0; i < engine->GetMaxClients(); ++i )
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

			if( fov < bestFov )
			{
				bestFov = fov;
				bestTarget = player;
			}
		}

		return bestTarget;
	};

	auto target = GetTargetEntity();
	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);

	if (!target)
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
		return true; //if left and right are equal and better than back

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
                CreateMove::sendPacket = AntiAim::bSend = false;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                CreateMove::sendPacket = AntiAim::bSend = false;
                lbyBreak = true;
                lastCheck = globalVars->curtime;
                needToFlick = true;
            }
        }
}

static void DefaultRageAntiAim(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    using namespace Settings::AntiAim::RageAntiAim;
    if (!localplayer || !localplayer->GetAlive())
        return;

    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (AntiAim::ManualAntiAim::alignLeft) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y += 70.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignBack) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 180.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignRight) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 70.f;
            return;
        }
    }

    float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (Settings::AntiAim::HeadEdge::enabled)    {
        
        // if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = GetBestHeadAngle(cmd);
        return;
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

    if(AntiAim::bSend)
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;

            case AntiAimFakeType_y::Static:
                AntiAim::fakeAngle.y = angle.y = inverted ? (angle.y-180.f)+GetPercentVal(maxDelta/2, AntiAImPercent) : (angle.y-180.f)-GetPercentVal(maxDelta/2, AntiAImPercent);
                break;

            case AntiAimFakeType_y::Jitter:
                static bool bFlip = false;
                bFlip = !bFlip;
                AntiAim::fakeAngle.y = angle.y = bFlip ?  (angle.y-180.f)-GetPercentVal(maxDelta/2, AntiAImPercent) : (angle.y-180.f)+GetPercentVal(maxDelta/2, AntiAImPercent);
                break;

            case AntiAimFakeType_y::Randome:
                AntiAim::fakeAngle.y = angle.y = (angle.y-180.f) - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/360.f);
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
                 AntiAim::realAngle.y = angle.y = inverted ? (angle.y-180.f)-GetPercentVal(maxDelta/2, AntiAImPercent) : (angle.y-180.f)+GetPercentVal(maxDelta/2, AntiAImPercent);
                break;

            case AntiAimRealType_Y::Jitter:
                static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (FakeLag::ticks == Settings::FakeLag::value-1){
                        bFlip = !bFlip;
                    }
                }
                else{
                        bFlip = !bFlip;
                }
                 AntiAim::realAngle.y = angle.y = bFlip ? (angle.y-180.f)+GetPercentVal(180.f, JitterPercent) : (angle.y-180.f)-GetPercentVal(180.f, JitterPercent);
                break;

            case AntiAimRealType_Y::Randome:
                 AntiAim::realAngle.y = angle.y = angle.y - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/360.f));
                    break;
        }
    }

    inverted ? LBYBreak(AntiAim::realAngle.y, angle, localplayer) : LBYBreak(AntiAim::realAngle.y, angle, localplayer);
    
}

static void FakeArrondReal(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    using namespace Settings::AntiAim::RageAntiAim;

    if (!localplayer || !localplayer->GetAlive())
        return;

    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (AntiAim::ManualAntiAim::alignLeft) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y += 90.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignBack) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 180.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignRight) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 90.f;
            return;
        }
    }

    
    if (Settings::AntiAim::HeadEdge::enabled){
        // if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = GetBestHeadAngle(cmd);
            return;
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

    

    if(AntiAim::bSend)
    {
        const float &maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE:
               AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;

            case AntiAimFakeType_y::Static:
                AntiAim::fakeAngle.y = angle.y = inverted ? (angle.y-180.f)+GetPercentVal(maxDelta, AntiAImPercent) : (angle.y-180.f)-GetPercentVal(maxDelta, AntiAImPercent); 
                break;

            case AntiAimFakeType_y::Jitter:
                static bool bFlip = false;
                bFlip = !bFlip;
                AntiAim::fakeAngle.y = angle.y = bFlip ? angle.y-GetPercentVal(180.f, JitterPercent) : angle.y+GetPercentVal(180.f, JitterPercent);
                break;

            case AntiAimFakeType_y::Randome:
                AntiAim::fakeAngle.y = angle.y = (angle.y-180.f) - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/360.f);
                break;
        }          

        LBYBreak(AntiAim::realAngle.y, angle, localplayer);
    }     
    
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;
                
            case AntiAimRealType_Y::Static:
                AntiAim::realAngle.y = angle.y -= 180.f;
                break;

            case AntiAimRealType_Y::Jitter:
               static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (FakeLag::ticks == Settings::FakeLag::value-1){
                        bFlip = !bFlip;
                    }
                }
                else{
                    bFlip = !bFlip;
                }
                AntiAim::realAngle.y = angle.y = bFlip ? angle.y+GetPercentVal(180.f, JitterPercent) : angle.y-GetPercentVal(180.f, JitterPercent);
                break;

            case AntiAimRealType_Y::Randome:
                AntiAim::realAngle.y = angle.y = angle.y - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/180.f));;
                break;
        }       
    }

}

static void RealArrondFake(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    if (!localplayer || !localplayer->GetAlive())
        return;

    using namespace Settings::AntiAim::RageAntiAim; using namespace AntiAim::ManualAntiAim;

    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (alignLeft) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y += 90.f;
            return;
        } else if (alignBack) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 180.f;
            return;
        } else if (alignRight) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 90.f;
            return;
        }
    }

    const float &maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (Settings::AntiAim::HeadEdge::enabled)    {
        // if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = GetBestHeadAngle(cmd);
            return;
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

    if(AntiAim::bSend)
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;
            case AntiAimFakeType_y::Static:
                AntiAim::fakeAngle.y = angle.y -= 180.f;
                break;
            case AntiAimFakeType_y::Jitter:
                static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (CreateMove::sendPacket){
                        bFlip = !bFlip;
                    }
                }
                else{
                        bFlip = !bFlip;
                }
                AntiAim::fakeAngle.y = angle.y = bFlip ? angle.y-GetPercentVal(180.f, JitterPercent) : angle.y+GetPercentVal(180.f, JitterPercent);
                break;
            case AntiAimFakeType_y::Randome:
                AntiAim::fakeAngle.y = angle.y = (angle.y-180.f) - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/360.f);
                break;
        }   
        inverted ? LBYBreak(AntiAim::realAngle.y, angle, localplayer) : LBYBreak(AntiAim::realAngle.y, angle, localplayer);
    }     
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;

            case AntiAimRealType_Y::Static:
                 AntiAim::realAngle.y = angle.y = inverted ? (angle.y-180.f)+GetPercentVal(maxDelta, AntiAImPercent) : (angle.y-180.f)-GetPercentVal(maxDelta, AntiAImPercent);
                break;

            case AntiAimRealType_Y::Jitter:
                static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (FakeLag::ticks == Settings::FakeLag::value-1){
                        bFlip = !bFlip;
                    }
                }
                else{
                        bFlip = !bFlip;
                }
                 AntiAim::realAngle.y = angle.y = bFlip ? angle.y-GetPercentVal(180.f, JitterPercent) : angle.y+GetPercentVal(180.f, JitterPercent);
                break;

            case AntiAimRealType_Y::Randome:
                 AntiAim::realAngle.y = angle.y = angle.y - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/360.f));
                    break;
        }
    }
}

static void SemiDirectionRageAntiAIim(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd)
{
    using namespace Settings::AntiAim::RageAntiAim;

    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (AntiAim::ManualAntiAim::alignLeft) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y += 90.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignBack) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 180.f;
            return;
        } else if (AntiAim::ManualAntiAim::alignRight) {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 90.f;
            return;
        }
    }

    const float &maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());
    if (Settings::AntiAim::HeadEdge::enabled)    {
        // if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = GetBestHeadAngle(cmd);
            return;
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

    if(AntiAim::bSend)
    {
        switch (Settings::AntiAim::Yaw::typeFake)
        {
            case AntiAimFakeType_y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;

            case AntiAimFakeType_y::Static:
                AntiAim::fakeAngle.y = angle.y = inverted ? AntiAim::realAngle.y-30.f : AntiAim::realAngle.y+30.f;
                break;

            case AntiAimFakeType_y::Jitter:
                static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (CreateMove::sendPacket){
                        bFlip = !bFlip;
                    }
                }
                else{
                        bFlip = !bFlip;
                }
                AntiAim::fakeAngle.y = angle.y = bFlip ?  AntiAim::realAngle.y-25.f : AntiAim::realAngle.y+25.f;
                break;

            case AntiAimFakeType_y::Randome:
                AntiAim::fakeAngle.y = angle.y = (angle.y-180.f) - static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX/360.f);
                break;
        }   
        inverted ? LBYBreak(AntiAim::realAngle.y, angle, localplayer) : LBYBreak(AntiAim::realAngle.y, angle, localplayer);
    }     
    else
    {     
        switch (Settings::AntiAim::Yaw::typeReal)
        {
            case AntiAimRealType_Y::NONE:
                AntiAim::fakeAngle = AntiAim::realAngle = angle;
                break;

            case AntiAimRealType_Y::Static:
                 AntiAim::realAngle.y = angle.y = inverted ? (angle.y-180.f)-GetPercentVal(maxDelta, AntiAImPercent) : (angle.y-180.f)+GetPercentVal(maxDelta, AntiAImPercent);
                break;

            case AntiAimRealType_Y::Jitter:
                static bool bFlip = false;
                if (Settings::FakeLag::enabled){
                    if (FakeLag::ticks == Settings::FakeLag::value-1){
                        bFlip = !bFlip;
                    }
                }
                else{
                        bFlip = !bFlip;
                }
                 AntiAim::realAngle.y = angle.y = bFlip ? angle.y-GetPercentVal(180.f, JitterPercent) : angle.y+GetPercentVal(180.f, JitterPercent);
                break;

            case AntiAimRealType_Y::Randome:
                 AntiAim::realAngle.y = angle.y = angle.y - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/360.f));
                    break;
        }
    }
}

static void FreeStand(C_BasePlayer *const localplayer, QAngle& angle, CUserCmd* cmd){

    using namespace AntiAim::ManualAntiAim;
    using namespace Settings::AntiAim::RageAntiAim;
    
    if (Settings::AntiAim::ManualAntiAim::Enable)
    {
        if (alignLeft)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y += 90.f;
            return;
        }
        else if (alignBack)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 180.f;
            return;
        }
        else if (alignRight)
        {
            AntiAim::realAngle.y = AntiAim::fakeAngle.y = angle.y -= 90.f;
            return;
        }
    }
    
    if (Settings::AntiAim::HeadEdge::enabled)    {
    //    if (GetBestHeadAngle(cmd, angle))
        float headAngle = GetBestHeadAngle(cmd);
        if (headAngle > 0)
            AntiAim::fakeAngle.y = AntiAim::realAngle.y = angle.y = GetBestHeadAngle(cmd);
            return;
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
            if ( !(cmd->tick_count%16) )
                bFlip = !bFlip ;
            angle.y = bFlip ? (angle.y-180.f)-GetPercentVal(360.f, JitterPercent) : (angle.y-180.f)+GetPercentVal(360.f, JitterPercent);
            break;
        case AntiAimRealType_Y::Static:
            angle.y -= 180.f;
            break;
        case AntiAimRealType_Y::NONE:
            break;
        default:
            break;
    }
    AntiAim::fakeAngle = AntiAim::realAngle = angle;
}

static void DoAntiAimX(QAngle& angle)   
{ 
        AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x = 89.f;
}

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

    static auto LBYBREAK([&](float& offset){
        static bool lbyBreak = false;
        static float lastCheck = 0.f;
        float vel2D = localplayer->GetVelocity().Length2D();
        if( vel2D >= 0.1f || !(localplayer->GetFlags() & FL_ONGROUND) || localplayer->GetFlags() & FL_FROZEN ){
                lbyBreak = false;
                lastCheck = globalVars->curtime;
            } 
            else {
                if( !lbyBreak && ( globalVars->curtime - lastCheck ) > 0.22 ){
                    AntiAim::realAngle.y = angle.y = offset;
                    AntiAim::bSend = false;
                    lbyBreak = true;
                    lastCheck = globalVars->curtime;
                    needToFlick = true;
                } else if( lbyBreak && ( globalVars->curtime - lastCheck ) > 1.1 ){
                        AntiAim::realAngle.y = angle.y = offset;
                        AntiAim::bSend = false;
                    lbyBreak = true;
                    lastCheck = globalVars->curtime;
                    needToFlick = true;
                }
            }
    });

    QAngle ViewAngle;
        engine->GetViewAngles( ViewAngle);
    static int tickCount = cmd->tick_count;
    static auto OverWatchProof([&](){
        if (!AntiAim::bSend)
        {
            tickCount = cmd->tick_count;
            AntiAim::realAngle.y = angle.y += inverted ? maxDelta : maxDelta*-1;
        }
        else
        {
            cmd->tick_count = tickCount;
            AntiAim::fakeAngle = angle = ViewAngle;
        }
            
    });

    static auto Experimental([&](){
        if (!AntiAim::bSend)
        {
            tickCount = cmd->tick_count;
            AntiAim::realAngle.y = localplayer->GetAnimState()->goalFeetYaw = inverted ? angle.y + maxDelta*3 : angle.y - maxDelta*3;
        }
        else
        {
            cmd->tick_count = tickCount;
            AntiAim::fakeAngle = angle = ViewAngle;
        }
            
    });

    switch (Settings::AntiAim::LegitAntiAim::legitAAtype)
    {
    case LegitAAType::OverWatchProof:
        OverWatchProof();
        break;
    case LegitAAType::Experimental:
        Experimental();
    default:
        break;
    }
}

static void DoManuaAntiAim(C_BasePlayer* localplayer, QAngle& angle)
{
    using namespace Settings::AntiAim::RageAntiAim;

    static bool Bpressed = false;
    
    if (!Settings::AntiAim::ManualAntiAim::Enable)
        return;

    if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !Bpressed )
	{
		AntiAim::ManualAntiAim::alignBack = !AntiAim::ManualAntiAim::alignBack;
		AntiAim::ManualAntiAim::alignLeft = AntiAim::ManualAntiAim::alignRight = false;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !Bpressed)
	{
		AntiAim::ManualAntiAim::alignRight = !AntiAim::ManualAntiAim::alignRight;
        AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignLeft = false;
	}	
    else if ( inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !Bpressed )
    {
		AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignRight = false;
        AntiAim::ManualAntiAim::alignLeft = !AntiAim::ManualAntiAim::alignLeft;
	}
    bool buttonNotPressed = !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::LeftButton ) && !inputSystem->IsButtonDown( Settings::AntiAim::ManualAntiAim::RightButton ) && !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton );	
    
    if (buttonNotPressed && Bpressed)
        Bpressed = false;
}

static bool canMove(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeweapon, CUserCmd* cmd)
{

    if (!activeweapon)
        return false;
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
    if (cmd->buttons & IN_ATTACK)
        return false;
    if ( cmd->buttons & IN_USE )
        return false;
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
        CreateMove::lastTickViewAngles = AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
        return;
    }

    if (Settings::FakeLag::enabled)
        AntiAim::bSend = CreateMove::sendPacket;      
    else
        AntiAim::bSend = !AntiAim::bSend;

    QAngle angle = cmd->viewangles;
    QAngle oldAngle = cmd->viewangles;
    float oldForward = cmd->forwardmove;
    float oldSideMove = cmd->sidemove;
    
    if( Settings::AntiAim::LBYBreaker::enabled ){
        LBYBreak(Settings::AntiAim::LBYBreaker::offset, angle, localplayer);
    }
    else if (Settings::AntiAim::RageAntiAim::enable) // responsible for reage anti aim or varity of anti aims .. 
    {
        DoManuaAntiAim(localplayer, angle);
        if (Settings::AntiAim::RageAntiAim::atTheTarget)
        {
            C_BasePlayer* lockedTarget = GetClosestEnemy(cmd);
            if (lockedTarget)
                angle = Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition());
        }

        // cvar->ConsoleDPrintf(XORSTR(" Before Changing : %f : %f \n"), localplayer->GetAnimState()->goalFeetYaw, angle.y);
        switch (Settings::AntiAim::RageAntiAim::Type)
        {
            case RageAntiAimType::DefaultRage:
                DefaultRageAntiAim(localplayer, angle, cmd);
                break;
            case RageAntiAimType::RealArroundFake :
                RealArrondFake(localplayer, angle, cmd);
                break;
            case RageAntiAimType::FakeArroundReal :
                FakeArrondReal(localplayer, angle, cmd);
                break;
            case RageAntiAimType::SemiDirection:
                SemiDirectionRageAntiAIim(localplayer, angle, cmd);
                break;
            case RageAntiAimType::FreeStand:
                FreeStand(localplayer, angle, cmd);
                break;
            default:
                break;
        }       

        // cvar->ConsoleDPrintf(XORSTR(" After Changing : %f : %f \n"), localplayer->GetAnimState()->goalFeetYaw, angle.y);
        // float trueDelta = NormalizeAsYaw(*localplayer->GetLowerBodyYawTarget() - angle.y);
        // cvar->ConsoleDPrintf(XORSTR("True delta : %f  \n"), trueDelta);
        // cvar->ConsoleDPrintf(XORSTR("Max Delta : %f  \n"), AntiAim::GetMaxDelta(localplayer->GetAnimState()));
        // float fixEye = (angle.y + *localplayer->GetLowerBodyYawTarget());
        // cvar->ConsoleDPrintf(XORSTR("Fix Eye : %f  \n"), fixEye);
        DoAntiAimX(angle); // changing the x View Angle
    }
    else if (Settings::AntiAim::LegitAntiAim::enable) // Responsible for legit anti aim activated when the legit anti aim is enabled
        DoLegitAntiAim(localplayer, angle, AntiAim::bSend, cmd);

    if (fabsf(cmd->sidemove) < 2.0f) {
        if (cmd->buttons & IN_DUCK)
            oldSideMove = (cmd->tick_count%2) ? 3.f : -3.f;
        else
            oldSideMove = (cmd->tick_count%2) ? 2.f : -2.f;
    }
    
        
    Math::NormalizeAngles(angle);
    // Math::ClampAngles(angle);
    CreateMove::lastTickViewAngles = AntiAim::realAngle;

    // FixMouseDeltas(cmd, angle, oldAngle);
    cmd->viewangles = angle;
    if (!Settings::FakeLag::enabled)
        CreateMove::sendPacket = AntiAim::bSend;
    // Math::ClampAngles(angle);
    Math::CorrectMovement(oldAngle, cmd, oldForward, oldSideMove);    
}
