#include "fakewalk.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))

/*
 * Found a glitch if I trigger fake lag for certain amount it glitches but it triggers after some time
 * 
 */
void FakeWalk::CreateMove(CUserCmd *cmd)
{

	FakeWalking = false;
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}
	if (cmd->buttons & IN_ATTACK) {
		return;
	}
	if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeWalk::Key)) {
		return;
	}
	FakeWalking = true;

	int maxTick;
	if (Settings::AntiAim::FakeWalk::Speed != 100) {
		maxTick = GetPercentVal(14, Settings::AntiAim::FakeWalk::Speed);
	} else {
		maxTick = 14;
	}

	if (FakeWalk::ticks >= maxTick) {
		CreateMove::sendPacket = true;
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
		FakeWalk::ticks = 0;
	} else {
		CreateMove::sendPacket = false;
	}

	FakeWalk::ticks++;

}
