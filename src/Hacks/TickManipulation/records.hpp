#pragma once

#include "../HeaderFiles.h"
#include "../AimBot/ragebot.hpp"

#include <vector>

namespace Records
{
	struct BestEnemy
	{
		C_BasePlayer *entity = nullptr;
		Vector BestSpot = Vector(0);
		int BestDamage = 0;
	};

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
		BestEnemy bestenemy;
	};

	inline std::vector<Records::TickInfo> Ticks;

	inline int SelectedRecords = INT32_MAX;

	float GetLerpTime();

	bool IsTickValid(float time);

	void RemoveInvalidTicks();

	void RegisterTicks();

	void RemoveBackTrackTicks(const int &maxTick);
};
