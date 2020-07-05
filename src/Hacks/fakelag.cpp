#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "autowall.h"

int ticksMax = 100;

static C_BasePlayer* GetClosestEnemy (CUserCmd* cmd)
{
    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
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

		if (!Settings::Ragebot::friendly && Entity::IsTeamMate(player, localplayer))
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


void FakeLag::CreateMove(CUserCmd* cmd)
{
	if (!Settings::FakeLag::enabled)
		return;

	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (localplayer->GetFlags() & FL_ONGROUND && Settings::FakeLag::adaptive)
		return;

	 C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());

	static Vector oldorigin;
	if (CreateMove::sendPacket)
		oldorigin = localplayer->GetAbsOrigin();

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
			C_BasePlayer* target = GetClosestEnemy(cmd);
			AutoWall::FireBulletData data;
			if (!target)
				CreateMove::sendPacket = FakeLag::ticks < 100 - Settings::FakeLag::value;
			else if (cmd->buttons & IN_ATTACK && activeWeapon->GetNextPrimaryAttack() >= globalVars->curtime)
			{
				CreateMove::sendPacket = true;
				FakeLag::ticks = 0;
			}
			else if (AutoWall::GetDamage(target->GetBonePosition(CONST_BONE_HEAD), localplayer, true, data) > 0 && !(cmd->buttons & IN_ATTACK))
				CreateMove::sendPacket = FakeLag::ticks < 100 - Settings::FakeLag::value;
			else
				CreateMove::sendPacket = FakeLag::ticks < 100 - Settings::FakeLag::value;
		}
		// else
		// 	CreateMove::sendPacket = ticks < 100 - Settings::FakeLag::value;
	}
	
	if (CreateMove::sendPacket)
	{
		if ( cmd->forwardmove || cmd->sidemove)
			oldorigin = localplayer->GetAbsOrigin();
	}
	else {
		oldorigin += (localplayer->GetVelocity().Length() * globalVars->interval_per_tick) * 1.5;
		localplayer->SetAbsOrigin( &oldorigin );
	}
		 
	FakeLag::ticks++;
}
