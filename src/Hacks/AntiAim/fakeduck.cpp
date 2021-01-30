#include "fakeduck.h"

void FakeDuck::CreateMove(CUserCmd *cmd)
{
	if (!Settings::AntiAim::FakeDuck::enabled) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey)) {
		FakeDuck::FakeDucking = false;
		return;
	}

	FakeDuck::FakeDucking = true;
	if (cmd->buttons & IN_ATTACK) {
		cmd->buttons &= ~IN_DUCK;
		CreateMove::sendPacket = true;
	}

	static bool counter = true;
	static int counters = 0;

	if (counters == 9) {
		counters = 0;
		counter = !counter;
	}

	counters++;

	if (counter) {
		cmd->buttons |= IN_BULLRUSH | IN_DUCK;
		if (counters == 9) {
			CreateMove::sendPacket = true;
		} else {
			CreateMove::sendPacket = false;
		}
	} else {
		cmd->buttons &= ~IN_DUCK;
		if (counters == 1) {
			CreateMove::sendPacket = true;
		} else {
			CreateMove::sendPacket = false;
		}
	}
}

void FakeDuck::OverrideView(CViewSetup *pSetup)
{
	if (!Settings::AntiAim::FakeDuck::enabled) {
		return;
	}

	if (!inputSystem->IsButtonDown(Settings::AntiAim::FakeDuck::fakeDuckKey)) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	pSetup->origin.z = localplayer->GetAbsOrigin().z + 64.0f;
}
