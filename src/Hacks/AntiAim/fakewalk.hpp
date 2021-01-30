#pragma once

#include "../HeaderFiles.h"

namespace FakeWalk
{
	inline bool FakeWalking = false;
	inline int ticks = 0;

	void CreateMove(CUserCmd *cmd);
}
