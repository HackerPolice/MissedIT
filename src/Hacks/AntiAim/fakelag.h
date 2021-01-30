#pragma once

#include "../HeaderFiles.h"

namespace FakeLag
{
	//Hooks
	void CreateMove(CUserCmd *cmd);

	inline int ticks = 0;
};
