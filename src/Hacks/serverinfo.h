#pragma once

#include "../SDK/IGameEvent.h"

namespace ServerInfo
{
	//Hooks
	void FireGameEvent(IGameEvent *event);

	inline int gameMode;
	inline bool isValveDS = true;
}
