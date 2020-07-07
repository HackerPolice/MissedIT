#include "animfix.h"

#include "../Utils/math.h"

void AnimFix::FrameStageNotify(ClientFrameStage_t stage)
{
    if (!engine->IsInGame())
        return;
    C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

    if (!localplayer || !localplayer->GetAlive())
        return;

    CCSGOAnimState* animstate = localplayer->GetAnimState();

    if (!animstate)
        return;
 

}