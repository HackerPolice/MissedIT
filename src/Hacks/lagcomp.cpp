#include "lagcomp.h"

// source from nimbus bcz i am lezy xd
// std::vector<LagComp::LagCompTickInfo> LagComp::lagCompTicks;

#define TICK_INTERVAL globalVars->interval_per_tick


#define TIME_TO_TICKS( dt )	( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

static void RemoveInvalidTicks()
{
	auto &records = LagComp::lagCompTicks;

	for (auto record = records.begin(); record != records.end(); record++)
	{
		if (!LagComp::IsTickValid(record->simulationTime))
		{
			records.erase(record);

			if (!records.empty())
				record = records.begin();
			else
				break;
		}
	}
}

static void RegisterTicks()
{
	const auto localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	const auto curTick = LagComp::lagCompTicks.insert(LagComp::lagCompTicks.begin(), {globalVars->tickcount, globalVars->curtime});
	int playerCount = engine->GetMaxClients();
	for (int i = playerCount; i > 0 ; i--)
	{
		auto player = (C_BasePlayer *)entityList->GetClientEntity(i);

		if (!player
		|| player == localplayer
		|| player->GetDormant()
		|| !player->GetAlive()
		|| Entity::IsTeamMate(player, localplayer)
		|| player->GetImmune())
			continue;

		LagComp::LagCompRecord record;

		record.entity = player;
		record.origin = player->GetVecOrigin();
		record.head = player->GetBonePosition(CONST_BONE_HEAD);

		if (player->SetupBones(record.bone_matrix, 128, 256, globalVars->curtime) )
			curTick->records.push_back(record);
	}
}

void LagComp::CreateMove(CUserCmd *cmd)
{
	if (!Settings::Ragebot::LagComp::enabled)
		return;

	RemoveInvalidTicks();
	RegisterTicks();

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	
	if (!localplayer || !localplayer->GetAlive())
		return;

	C_BaseCombatWeapon *weapon = (C_BaseCombatWeapon *)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
	if (!weapon)
		return;

	float serverTime = localplayer->GetTickBase() * globalVars->interval_per_tick;

	QAngle angle;
	engine->GetViewAngles(angle);
	QAngle rcsAngle = angle + *localplayer->GetAimPunchAngle();

	if (cmd->buttons & IN_ATTACK && weapon->GetNextPrimaryAttack() <= serverTime)
	{
		float fov = 180.0f;

		static int tickcount = cmd->tick_count;
		Vector absOrigin;
		C_BasePlayer* entity = nullptr;
		bool has_target = false;

		for (auto &&Tick : LagComp::lagCompTicks)
		{
			for (auto &record : Tick.records)
			{
				float tmpFOV = Math::GetFov(rcsAngle, Math::CalcAngle(localplayer->GetEyePosition(), record.head));

				if (tmpFOV < fov)
				{
					fov = tmpFOV;
					tickcount = TIME_TO_TICKS(record.entity->GetSimulationTime() + LagComp::GetLerpTime());
					absOrigin = record.origin;
					entity = record.entity;
					has_target = true;
					break;
				}
			}
			if (has_target) break;
		}
			
		cmd->tick_count = tickcount;
	}
}