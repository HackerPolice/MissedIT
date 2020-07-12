#pragma once

#include "autowall.h"

#include "../../SDK/IInputSystem.h"
#include "../../SDK/IGameEvent.h"
#include "../../SDK/definitions.h"
#include "../../Utils/xorstring.h"
#include "../../Utils/math.h"
#include "../../Utils/entity.h"
#include "../../Utils/bonemaps.h"
#include "../../settings.h"
#include "../../interfaces.h"
#include "../../Hooks/hooks.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>
#include <cstdint>

namespace Legitbot
{
	extern bool aimStepInProgress;
	extern std::vector<int64_t> friends;
    extern int targetAimbot;
	inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;

	//Hooks
	void CreateMove(CUserCmd* cmd);
	void FireGameEvent(IGameEvent* event);
}
