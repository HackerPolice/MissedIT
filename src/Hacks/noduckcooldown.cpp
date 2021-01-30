#include "noduckcooldown.h"

#include "../settings.h"
#include "../interfaces.h"

void NoDuckCooldown::CreateMove(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	cmd->buttons |= IN_BULLRUSH;
}
