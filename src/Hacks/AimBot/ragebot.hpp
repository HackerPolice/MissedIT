#pragma once

#include "../HeaderFiles.h"
#include "autowall.h"
#include "../TickManipulation/lagcomp.hpp"
#include "../../Utils/draw.h"

namespace Ragebot {
    struct DATA 
    {
        C_BasePlayer* player = nullptr;
        Vector PrevTickEyePosition = Vector(0);
        bool shooted = false;
        int playerhealth = 0;
        bool autoslow = false;
        QAngle prevAngle = QAngle(0);
        bool needToStop = false;
        bool Hitted = false;
        bool IHit = false;
    };

    typedef struct DATA Data;
    
    inline Data data;
    inline Vector localEye = Vector(0);
    inline Vector BestSpot = Vector(0);
    inline int BestDamage = 0;
    inline int ShootEnemyIndex = 0;
    inline int prevDamage = 0;
    inline matrix3x4_t BoneMatrix[128];

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
    void FireGameEvent2(IGameEvent* event);

    /*  
     * Yo 
     */ 
    inline C_BasePlayer* enemy = nullptr;
	inline C_BasePlayer* localplayer = nullptr;
    inline C_BaseCombatWeapon* activeWeapon = nullptr;
    inline Vector bulPosition;
    inline RageWeapon_t *currentWeaponSetting;


    void init(C_BasePlayer*, C_BaseCombatWeapon*);
}

// extern WriteUserCmdFn WriteUserCmd;