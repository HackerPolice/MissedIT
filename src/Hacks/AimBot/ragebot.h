#pragma once

#include "../HeaderFiles.h"
#include "autowall.h"
#include "../lagcomp.h"

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

extern WriteUserCmdFn WriteUserCmd;

