#include "skateboarding.h"

#include "../settings.h"
#include "../interfaces.h"

/**
 * Created by Acuifex, pasted by winston xD
 * https://github.com/acuifex/acuion/blob/master/src/Hacks/keyfix.cpp
 */
void Skateboarding::CreateMove(CUserCmd* cmd){

    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->IsAlive())
        return;

    if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP)
        return;

    cmd->buttons &= ~IN_BACK; // reset keys
    cmd->buttons &= ~IN_FORWARD;
    cmd->buttons &= ~IN_MOVELEFT;
    cmd->buttons &= ~IN_MOVERIGHT;

    if (Settings::Skateboarding::enabled) { // do skateing
        if (cmd->forwardmove > 0)
            cmd->buttons |= IN_BACK;
        else if (cmd->forwardmove < 0)
            cmd->buttons |= IN_FORWARD;

        if (cmd->sidemove > 0)
            cmd->buttons |= IN_MOVELEFT;
        else if (cmd->sidemove < 0)
            cmd->buttons |= IN_MOVERIGHT;
    }
    else { // fix skateboarding
        if (cmd->forwardmove > 0)
            cmd->buttons |= IN_FORWARD;
        else if (cmd->forwardmove < 0)
            cmd->buttons |= IN_BACK;

        if (cmd->sidemove > 0)
            cmd->buttons |= IN_MOVERIGHT;
        else if (cmd->sidemove < 0)
            cmd->buttons |= IN_MOVELEFT;
    }

}