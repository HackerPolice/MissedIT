#include "fakelag.h"

#ifndef absol
#define absol(x) x < 0 ? x*-1 : x
#endif

void FakeLag::CreateMove(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive()) {
		return;
	} else if (FakeLag::ticks >= Settings::FakeLag::value) {
		CreateMove::sendPacket = true;
		FakeLag::ticks = Settings::FakeLag::impulseLag ? -16 : -1;
	} else if (FakeLag::ticks > 0) {
		CreateMove::sendPacket = false;
	}

	FakeLag::ticks++;

}
