/*
 File to handle Rapid Fire to fire in one tick Don't Know If i success or not
 But This cheat really need a rapid fire god give me strenth
 */


#include "fakelag2.hpp"

void RapidFire::CreateMove(CUserCmd* cmd){

    // Dumb Method
    if (!Settings::FakeLag::enabled)
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
		  return;
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
		  return;
    
    static int curTickCount = 0;

    // if (cmd->buttons & IN_ATTACK && curTickCount == 0){
    //   cmd->tick_count = INT64_MAX;
    //   curTickCount++;
    //   CreateMove::sendPacket = false;
    // }else if (curTickCount > 0 && activeWeapon->GetNextPrimaryAttack() > globalVars->curtime){
    //   CreateMove::sendPacket = false;
    //   // curTickCount++;
    // }else if (curTickCount > 0 && activeWeapon->GetNextPrimaryAttack() < globalVars->curtime)
    // {
    //   CreateMove::sendPacket = true;
    //   curTickCount = 0;
    // }
}