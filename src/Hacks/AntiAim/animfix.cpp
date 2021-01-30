#include "animfix.h"

void AnimFix::FrameStageNotify(ClientFrameStage_t stage)
{
	return;
	if (!engine->IsInGame()) {
		return;
	}
	if (stage != ClientFrameStage_t::FRAME_RENDER_START) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	CCSGOAnimState *animstate = localplayer->GetAnimState();

	if (!animstate) {
		return;
	}

	static matrix3x4_t matrix[128];
	static std::array<float, 24> poses;
	static std::array<AnimationLayer, 15> *m_layers;

	if (animstate->lastAnimUpdateFrame == globalVars->framecount) {
		animstate->lastAnimUpdateFrame -= 1;
	}

	animstate->lastAnimUpdateTime = std::max(0.0f, globalVars->curtime -
	                                               animstate->lastAnimUpdateFrame); // negative values possible when clocks on client and server go out of sync..

	// get layers.
	m_layers = localplayer->GetAnimOverlay();
	localplayer->updateClientAnimation();

	static float absyaw;
	if (animfix) {
		if (CreateMove::sendPacket) {
			absyaw = animstate->goalFeetYaw;
			poses = localplayer->GetPoseParam();
		}
		m_layers->at(12).m_flWeight = 0.f;
		animfix = false;
	}

	localplayer->SetAimlayer(m_layers);
	localplayer->SetPoseParam(&poses);

	// update our real rotation.
	// *localplayer->Getlowe() = QAngle(0, absyaw, 0);
	*localplayer->GetLowerBodyYawTarget() = absyaw;

	// build bones.
	localplayer->SetupBones(matrix, 128, 256, localplayer->GetSimulationTime());

}
