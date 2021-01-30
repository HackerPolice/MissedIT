#include "backtrack.hpp"

// source from nimbus bcz i am lezy xd
#define TICK_INTERVAL            (globalVars->interval_per_tick)
#define TIME_TO_TICKS(dt)        ( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME(t)        ( TICK_INTERVAL *( t ) )

static C_BasePlayer *GetClosestEnemy(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return nullptr;
	}

	C_BasePlayer *closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	int maxPlayers = engine->GetMaxClients();
	for (int i = 1; i < maxPlayers; ++i) {
		C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player
		    || player == localplayer
		    || player->GetDormant()
		    || !player->IsAlive()
		    || player->GetImmune()) {
			continue;
		}

		if (Entity::IsTeamMate(player, localplayer)) {
			continue;
		}

		Vector cbVecTarget = player->GetAbsOrigin();

		float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

		if (prevFOV == 0.f) {
			prevFOV = cbFov;
			closestPlayer = player;
		} else if (cbFov < prevFOV) {
			prevFOV = cbFov;
			closestPlayer = player;
		}
	}
	return closestPlayer;
}

void BackTrack::CreateMove(CUserCmd *cmd)
{
	Records::RemoveInvalidTicks();
	Records::RegisterTicks();

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	C_BaseCombatWeapon *weapon = (C_BaseCombatWeapon *) entityList->GetClientEntityFromHandle(
			localplayer->GetActiveWeapon());
	if (!weapon) {
		return;
	}

	C_BasePlayer *closestEnemy = nullptr;

	if (Ragebot::data.player && Ragebot::data.player->IsAlive()) {
		closestEnemy = Ragebot::data.player;
	} else {
		closestEnemy = GetClosestEnemy(cmd);
	}

	if (!closestEnemy) {
		return;
	}

	int size = Records::Ticks.size() - 1;
	static int index;
	bool has_target = false;

	for (index = size; index > -1; index--) {
		const auto &tick = Records::Ticks.at(index);
		for (auto &record : tick.records) {
			if (record.entity == closestEnemy) {
				cmd->tick_count = tick.tickCount;
				Records::SelectedRecords = index;
				has_target = true;
				break;
			}
		}
		if (has_target) {
			break;
		}
	}
}
