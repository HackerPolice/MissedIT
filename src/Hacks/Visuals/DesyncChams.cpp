/*
 * Hope will able to create that fuking desync chams shit
 */
#include "DesyncChams.hpp"

void DsyncChams::FrameStageNotify(ClientFrameStage_t stage)
{

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	// localplayer->SetupBones()
	if (stage == ClientFrameStage_t::FRAME_START) {
		if (Settings::ThirdPerson::toggled) {
			// QAngle backUpVAngle;
			// float lower;

			// backUpVAngle = *localplayer->GetVAngles();
			// lower = *localplayer->GetLowerBodyYawTarget();

			// *localplayer->GetVAngles() = CreateMove::lastTickViewAngles;
			// *localplayer->GetLowerBodyYawTarget() = localplayer->GetVAngles()->y + 30;
			// // Fra
			// localplayer->updateClientAnimation();
			// localplayer->SetupBones(fakeboneMatrix, 128, 128, globalVars->curtime);

			// *localplayer->GetVAngles() = backUpVAngle;
			// *localplayer->GetLowerBodyYawTarget() = lower;
			// localplayer->updateClientAnimation();
		}
	}
	if (stage == ClientFrameStage_t::FRAME_RENDER_START) {
		if (Settings::ThirdPerson::toggled) {
			*localplayer->GetVAngles() = CreateMove::lastTickViewAngles;
		}
	}
}

void DsyncChams::CreateMove(CUserCmd *cmd)
{
	// C_BasePlayer *localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	// if (!localplayer || !localplayer->IsAlive())
	//     return;

	// localplayer->GetAnimState()->currentFeetYaw = localplayer->GetVAngles()->y + 30;
	// localplayer->SetupBones(fakeboneMatrix, 128, 128, globalVars->curtime);
}
