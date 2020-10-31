#include "fakewalk.hpp"
#include "../AimBot/ragebot.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))

void FakeWalk::CreateMove(CUserCmd* cmd){
	
	FakeWalking = true;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer || !localplayer->GetAlive()){
		return;
	}   
	else if ( cmd->buttons & IN_ATTACK ){
		return;
	}
    else if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeWalk::Key)){    
		return;
	}
	FakeWalking = false;
	if (ticks > 13)
	{
		ticks = 0;
	}
	else 
	{
		ticks++;
	}

	

	int maxTick = GetPercentVal(14, Settings::AntiAim::FakeWalk::Speed);
	bool canMove = !ticks || ticks > maxTick;

	if (canMove)
	{
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
	}	
   	CreateMove::sendPacket = !ticks;
}