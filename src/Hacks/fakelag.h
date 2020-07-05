#pragma once

#include "../SDK/IInputSystem.h"

namespace FakeLag
{
	//Hooks
	void CreateMove(CUserCmd* cmd);
	inline bool dt = false;
	inline int ticks = 0;
	inline int dtTickCount = 0;
};
