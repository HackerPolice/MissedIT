#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "AimBot/autowall.h"

#include <float.h>

int ticksMax = 60;

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

			CreateMove::sendPacket = !(cmd->tick_count%packetsToChoke);
		}
		else if ( (cmd->sidemove > 2 || cmd->sidemove < -2) && cmd->forwardmove == 0)
		{
			FakeLag::ticks = 0;
			CreateMove::sendPacket = FakeLag::ticks <= 60 ? false : true;
		}
		else
		{
			CreateMove::sendPacket = FakeLag::ticks <= Settings::FakeLag::value ? false : true;
		}
	}
	
	if (CreateMove::sendPacket)
	{
		FakeLag::ticks = 0;
		oldorigin = (localplayer->GetVelocity().Length2D() * globalVars->interval_per_tick);
		localplayer->SetAbsOrigin( &oldorigin );
		oldorigin = localplayer->GetAbsOrigin();
		// cmd->forwardmove = FLT_MAX;
		// cmd->sidemove = FLT_MAX;
	}
	
	
	FakeLag::ticks++;
}
