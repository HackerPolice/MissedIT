#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include "../settings.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../interfaces.h"
#include "../Utils/xorstring.h"
#include "../hooker.h"
#include "../Hooks/hooks.h"
#include "../Utils/bonemaps.h"
#include "../Utils/math.h"
#include "autowall.h"
#include "lagcomp.h"

namespace Ragebot {

    struct enemy 
    {
        C_BasePlayer* player = nullptr;
        int LockedBone = -1;
        int bestDamage = 0;
        Vector lockedSpot = Vector(0);
    };

    struct Miss
    {
        bool shooted = false;
        int playerhelth = 0;
    };

    inline enemy lockedEnemy;
    inline Miss miss;
    inline Vector localEye = Vector(0),
    	 BestSpot = Vector(0);
    inline int BestDamage = 0;

    inline bool shouldAim = false,
                shouldSlow = false,
                doubleTap = false;
    extern std::vector<int64_t> friends;

    inline int prevDamage = 0, dtTick_Count = 0, dtTick_Need = 0;
    inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
}


