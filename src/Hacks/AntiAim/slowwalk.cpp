#include "slowwalk.hpp"

#define GetPercentVal(val, percent) (val * (percent/100.f))

void SlowWalk::CreateMove(CUserCmd *cmd)
{

	SlowWalking = false;
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}
	if (!inputSystem->IsButtonDown(Settings::AntiAim::SlowWalk::Key)) {
		return;
	}

	SlowWalking = true;
	QAngle ViewAngle;
	engine->GetViewAngles(ViewAngle);

	static Vector oldOrigin = localplayer->GetAbsOrigin();
	Vector velocity = (localplayer->GetVecOrigin() - oldOrigin)
	                  * (1.f / globalVars->interval_per_tick);
	oldOrigin = localplayer->GetAbsOrigin();
	float speed = velocity.Length();

	if (speed > Settings::AntiAim::SlowWalk::Speed) {
		cmd->forwardmove = 0;
		cmd->sidemove = 0;
		CreateMove::sendPacket = false;
	} else {
		CreateMove::sendPacket = true;
	}
}
