#include "fakewalk.hpp"
#include "../AimBot/ragebot.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))


/*
 * FOund a glithch if I trigger fake lag for certain amount it glitches but it triggers after some time
 * 
 */
void FakeWalk::CreateMove(CUserCmd* cmd){
	
	FakeWalking = false;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->IsAlive()){
		return;
	}   
	else if ( cmd->buttons & IN_ATTACK ){
		return;
	}
    else if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeWalk::Key)){    
		return;
	}
	FakeWalking = true;

	

	int maxTick = GetPercentVal(14, Settings::AntiAim::FakeWalk::Speed);
	bool canMove = ticks >= maxTick;

	if (canMove)
	{
		CreateMove::sendPacket = true;
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
		ticks = 0;
	}else {
		CreateMove::sendPacket = false;
	}
   	
}