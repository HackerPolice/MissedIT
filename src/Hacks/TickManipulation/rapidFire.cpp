/*
 File to handle Rapid Fire to fire in one tick Don't Know If i success or not
 But This cheat really need a rapid fire god give me strenth
 */


#include "rapidFire.hpp"

void RapidFire::CreateMove(CUserCmd* cmd){

    // Dumb Method
    if (!engine->IsInGame() || !Settings::Ragebot::enabled)
        return;

    C_BasePlayer* localplayer = (C_BasePlayer*)entityList->GetClientEntity(engine->GetLocalPlayer());
    if (!localplayer)
		return;
    C_BaseCombatWeapon* activeWeapon = (C_BaseCombatWeapon*)entityList->GetClientEntityFromHandle(localplayer->GetActiveWeapon());
    if (!activeWeapon)
		return;

    // static int curTickCount = 0;

    // if (cmd->buttons & IN_ATTACK){
    //     cmd->tick_count += 13;
    //     curTickCount = cmd->tick_count;
    // }else if (activeWeapon->GetNextPrimaryAttack() < globalVars->curtime){
    //     CreateMove::sendPacket = false;
    // }else
    // {
    //     cmd->buttons |= IN_ATTACK;
    //     cvar->ConsoleDPrintf("Shifted \n");
    // }
    
}