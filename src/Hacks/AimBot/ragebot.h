#pragma once

#include "../DefaultHeader.hpp"


namespace Ragebot {

    struct enemy 
    {
        C_BasePlayer* player = nullptr;
        int LockedBone = -1;
        int bestDamage = 0;
        Vector lockedSpot = Vector(0);
    };

    inline Vector localEye = Vector(0),
    	 BestSpot = Vector(0);
    inline int BestDamage = 0;

    inline bool shouldAim = false,
                shouldSlow = false,
                doubleTap = false;
    extern std::vector<int64_t> friends;
    extern int targetAimbot;
    inline int prevDamage = 0, dtTick_Count = 0, dtTick_Need = 0;
    inline ItemDefinitionIndex prevWeapon = ItemDefinitionIndex::INVALID;

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
}