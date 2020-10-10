#include "slowwalk.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))

void SlowWalk::CreateMove(CUserCmd* cmd){

    if (!Settings::AntiAim::SlowWalk::enabled)
        return;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive())
		return;
	if (Ragebot::data.player)
		return;
    if (!inputSystem->IsButtonDown(Settings::AntiAim::SlowWalk::Key))
		return;
        
    QAngle ViewAngle;
	engine->GetViewAngles(ViewAngle);
	    
    static Vector oldOrigin = localplayer->GetAbsOrigin( );
	Vector velocity = ( localplayer->GetVecOrigin( )-oldOrigin ) 	
							* (1.f/globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin( );
	float speed  = velocity.Length( );
	
    if(speed > Settings::AntiAim::SlowWalk::Speed )
	{
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
	}
}