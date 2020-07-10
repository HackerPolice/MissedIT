#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "AimBot/autowall.h"

int ticksMax = 16;

void FakeLag::CreateMove(CUserCmd* cmd)
{
	if (!Settings::FakeLag::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if ( !(localplayer->GetFlags() & FL_ONGROUND) && Settings::FakeLag::adaptive)
		return;

	static Vector oldorigin = localplayer->GetAbsOrigin();

	if (FakeLag::ticks >= ticksMax)
	{
		CreateMove::sendPacket = true;
		FakeLag::ticks = 0;
	}
	else
	{
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

			CreateMove::sendPacket = FakeLag::ticks < 16 - packetsToChoke;
		}
		else
		{
			CreateMove::sendPacket = FakeLag::ticks < 16 - Settings::FakeLag::value;
		}
	}
	
	if (CreateMove::sendPacket)
	{
		oldorigin = localplayer->GetAbsOrigin();
		localplayer->GetAnimState()->origin = (localplayer->GetVelocity().Length() * globalVars->interval_per_tick * Settings::FakeLag::value);
		localplayer->SetAbsOrigin( &oldorigin );
	}
	

	FakeLag::ticks++;
}
