#pragma once

#include "../HeaderFiles.h"

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
    extern int targetAimbot;

	//Hooks
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);
}
