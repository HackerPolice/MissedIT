#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"

#ifndef absol
	#define absol(x) x < 0 ? x*-1 : x
#endif

int ticksMax = 14;

void FakeLag::CreateMove(CUserCmd* cmd)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if ( FakeLag::ticks > ticksMax){
		FakeLag::ticks = 0;
	}

	if (Settings::FakeLag::adaptive)
	{
		int packetsToChoke;
		if (localplayer->GetVelocity().Length() > 0.f)
		{
			packetsToChoke = (int)((64.f / globalVars->interval_per_tick) / localplayer->GetVelocity().Length()) + 1;
			if (packetsToChoke >= 15)
				packetsToChoke = 14;
			if (packetsToChoke < Settings::FakeLag::value)
				packetsToChoke = Settings::FakeLag::value;
		}
		else
			packetsToChoke = 0;
		
		CreateMove::sendPacket = FakeLag::ticks < (16 - packetsToChoke);
	}
	else{
		CreateMove::sendPacket = FakeLag::ticks >= Settings::FakeLag::value;
	}

	FakeLag::ticks++;
}
