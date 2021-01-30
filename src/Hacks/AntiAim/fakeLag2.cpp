#include "fakelag2.hpp"

/**
 * @brief Allow to manupulate fakelab after the player movement done so be creative with it
 */

void FakeLag2::CreateMove(CUserCmd *cmd)
{

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	if (Settings::FakeLag::InAir::Enable && !(localplayer->GetFlags() & FL_ONGROUND)) {
		if (FakeLag2::Ticks >= Settings::FakeLag::InAir::Value) {
			CreateMove::sendPacket = true;
			FakeLag2::Ticks = 0;
		} else {
			CreateMove::sendPacket = false;
			FakeLag2::Ticks++;
		}
	} else if (Settings::FakeLag::OnShot::Enable && (cmd->buttons & IN_ATTACK) && !FakeLag2::Shooted) {
		CreateMove::sendPacket = false;
		FakeLag2::Ticks = 0;
		FakeLag2::Shooted = true;
	} else if (Settings::FakeLag::AfterShot::Enable && cmd->buttons & IN_ATTACK && !FakeLag2::Shooted) {
		FakeLag2::Ticks = 0;
		FakeLag2::Shooted = true;
	} else if (FakeLag2::Ticks < Settings::FakeLag::OnShot::Value && FakeLag2::Shooted) {
		CreateMove::sendPacket = false;
		FakeLag2::Ticks++;
	} else {
		FakeLag2::Shooted = false;
	}

}
