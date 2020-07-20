#pragma once

#include "../SDK/CBaseClientState.h"
#include "../SDK/IInputSystem.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../interfaces.h"
#include "../settings.h"

#include <vector>

#ifndef CLAMP
    #define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

namespace LagComp
{

	struct LagCompRecord
	{
		C_BasePlayer *entity;
		Vector head, origin;
		matrix3x4_t bone_matrix[128];
	};

	// stores information about all players for one tick
	struct LagCompTickInfo
	{
		int tickCount;
		float simulationTime;
		std::vector<LagCompRecord> records;
	};

	void CreateMove(CUserCmd *cmd);

	inline std::vector<LagComp::LagCompTickInfo> lagCompTicks;
	inline float GetLerpTime();
	inline bool IsTickValid(float time);
	// inline void RemoveInvalidTicks();

} // namespace LagComp

inline float LagComp::GetLerpTime()
{
	int updateRate = cvar->FindVar("cl_updaterate")->GetInt();
	ConVar *minUpdateRate = cvar->FindVar("sv_minupdaterate");
	ConVar *maxUpdateRate = cvar->FindVar("sv_maxupdaterate");

	if (minUpdateRate && maxUpdateRate)
		updateRate = maxUpdateRate->GetInt();

	float ratio = cvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = cvar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = cvar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = cvar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = CLAMP(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return std::max(lerp, (ratio / updateRate));
}

inline bool LagComp::IsTickValid(float time) // pasted from polak getting some invalid ticks need some fix
{
	float correct = 0;

	correct += LagComp::GetLerpTime();
	correct = CLAMP(correct, 0.f, cvar->FindVar("sv_maxunlag")->GetFloat());

	float deltaTime = correct - (globalVars->curtime - time);

	if (fabsf(deltaTime) < 0.2f)
		return true;

	return false;
}


