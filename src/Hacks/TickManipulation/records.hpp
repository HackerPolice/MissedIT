#pragma once

#include "../HeaderFiles.h"

#include <vector>

namespace Records
{
	struct Record
	{
		C_BasePlayer *entity;
		int simulationTime;
		Vector head, origin;
		matrix3x4_t bone_matrix[128];
	};

	// stores information about all players for one tick
	struct TickInfo
	{
		int tickCount;
		float simulationTime;
		std::vector<Record> records;
	};

	inline std::vector<Records::TickInfo> Ticks;

	float GetLerpTime();
	bool IsTickValid(float time);
	void RemoveInvalidTicks();
	void RegisterTicks();
	void RemoveBackTrackTicks(const int &maxTick);
};