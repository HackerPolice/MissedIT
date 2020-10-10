#include "lagcomp.hpp"

#include "../AimBot/ragebot.hpp"

/*
 * Thanks to the Nimbus(in github) Project for the initial code
 */
#ifndef CLAMP
    #define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

#define TICK_INTERVAL			(globalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

void LagComp::CreateMove(CUserCmd *cmd)
{
	if (!Settings::BackTrack::enabled){
		Records::RemoveInvalidTicks();
		Records::RegisterTicks();
	}

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	C_BaseCombatWeapon *weapon = (C_BaseCombatWeapon *)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!weapon)
		return;

	float serverTime = localplayer->GetTickBase() * globalVars->interval_per_tick;

	if ( !(cmd->buttons & IN_ATTACK) || (weapon->GetNextPrimaryAttack() > serverTime) )
		return;
	if (!Ragebot::data.player || !Ragebot::data.player->GetAlive())
		return;
		
	bool has_target = false;

	for ( auto &Tick : Records::Ticks){
		for ( auto &record : Tick.records){
			if (record.entity == Ragebot::data.player){
				cmd->tick_count = TIME_TO_TICKS(record.simulationTime + Records::GetLerpTime());
				has_target = true;
				break;
			}
		}
		if (has_target){
			break;
		}
	}
}