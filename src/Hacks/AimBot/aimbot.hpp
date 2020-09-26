#pragma once

#include "../HeaderFiles.h"
class Aimbot {

    #define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
    
private:
    QAngle RCSLastPunch;

public:
    Aimbot(){}

    void NoRecoil(QAngle& angle, CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings);

    void AutoCrouch(C_BasePlayer* localplayer, CUserCmd* cmd, C_BaseCombatWeapon* activeWeapon, const RageWeapon_t& currentSettings);

    void AutoSlow(C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, float& forrwordMove, float& sideMove, QAngle& angle, const RageWeapon_t& currentSettings);

    void AutoR8(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd,Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings);

    void AutoShoot(C_BasePlayer* player, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon, CUserCmd* cmd, Vector& bestspot, QAngle& angle, float& forrwordMove, float& sideMove, const RageWeapon_t& currentSettings);

    void FixMouseDeltas(CUserCmd* cmd, C_BasePlayer* player, QAngle& angle, QAngle& oldAngle);

    bool canShoot(CUserCmd* cmd, C_BasePlayer* localplayer, C_BaseCombatWeapon* activeWeapon,Vector &bestSpot, C_BasePlayer* enemy,const RageWeapon_t& currentSettings);
};
