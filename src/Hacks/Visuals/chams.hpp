#pragma once

#include "../../SDK/vector.h"
#include "../../SDK/IVModelRender.h"

namespace Chams
{
	inline int BackTrackTicks = 0;
	inline matrix3x4_t BodyBoneMatrix[128];

	//Hooks
	void DrawModelExecute(void *thisptr, void *context, void *state, const ModelRenderInfo_t &pInfo,
	                      matrix3x4_t *pCustomBoneToWorld);
}
