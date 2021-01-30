#pragma once

#include "../HeaderFiles.h"

namespace DsyncChams
{

	inline matrix3x4_t fakeboneMatrix[128];
	inline bool Desync = true;

	void FrameStageNotify(ClientFrameStage_t stage);

	void DrawModelExecute(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo,
	                      matrix3x4_t *pCustomBoneToWorld);

	void CreateMove(CUserCmd *cmd);

};
