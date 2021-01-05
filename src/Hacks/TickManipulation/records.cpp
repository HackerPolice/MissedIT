#include "records.hpp"

#ifndef CLAMP
#define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

float Records::GetLerpTime()
{
	int updateRate = cvar->FindVar("cl_updaterate")->GetInt();
	ConVar *minUpdateRate = cvar->FindVar("sv_minupdaterate");
	ConVar *maxUpdateRate = cvar->FindVar("sv_maxupdaterate");

	if (minUpdateRate && maxUpdateRate) {
		updateRate = maxUpdateRate->GetInt();
	}

	float ratio = cvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0) {
		ratio = 1.0f;
	}

	float lerp = cvar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = cvar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = cvar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1) {
		ratio = CLAMP(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());
	}

	return std::max(lerp, (ratio / updateRate));
}

bool Records::IsTickValid(float time) // pasted from polak getting some invalid ticks need some fix
{
	float correct = 0;

	correct += Records::GetLerpTime();
	correct = CLAMP(correct, 0.f, cvar->FindVar("sv_maxunlag")->GetFloat());

	float deltaTime = correct - (globalVars->curtime - time);

	if (fabsf(deltaTime) < 0.2f) {
		return true;
	}

	return false;
}

void Records::RemoveInvalidTicks()
{
	auto &Ticks = Records::Ticks;

	for (auto Tick = Ticks.begin(); Tick != Ticks.end(); Tick++) {
		if (!Records::IsTickValid(Tick->simulationTime)) {
			Ticks.erase(Tick);

			if (!Ticks.empty()) {
				Tick = Ticks.begin();
			} else {
				break;
			}
		}
	}
}

void Records::RegisterTicks()
{
	const auto localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	const auto curTick = Records::Ticks.insert(Records::Ticks.begin(), {globalVars->tickcount, globalVars->curtime});

	for (int i = engine->GetMaxClients(); i > 1; i--) {
		const auto player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player
		    || player == localplayer
		    || player->GetDormant()
		    || !player->GetAlive()
		    || Entity::IsTeamMate(player, localplayer)
		    || player->GetImmune()) {
			continue;
		}

		Records::Record record;

		record.entity = player;
		record.simulationTime = player->GetSimulationTime();
		record.origin = player->GetVecOrigin();
		record.head = player->GetBonePosition(BONE_HEAD);

		if (player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX, globalVars->curtime)) {
			curTick->records.push_back(record);
		}
	}
}

void Records::RemoveBackTrackTicks(const int &maxTick)
{
	int size;
	size = Records::Ticks.size() - 1;
	if (size >= maxTick) {
		Records::Ticks.erase(Ticks.begin());
	}
}
