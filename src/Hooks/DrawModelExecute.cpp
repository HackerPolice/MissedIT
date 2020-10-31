#include "hooks.h"

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/chams.h"
#include "../Hacks/esp.h"
#include "../Hacks/AntiAim/fakeduck.h"
#include "../Hacks/AntiAim/antiaim.h"
#include "../Hacks/Visuals/DesyncChams.hpp"

typedef void (*DrawModelExecuteFn) (void*, void*, void*, const ModelRenderInfo_t&, matrix3x4_t*);

void Hooks::DrawModelExecute(void* thisptr, void* context, void *state, const ModelRenderInfo_t &pInfo, matrix3x4_t* pCustomBoneToWorld)
{
	if (Settings::ESP::enabled && (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot()))
	{
		Chams::DrawModelExecute(thisptr, context, state, pInfo, pCustomBoneToWorld);
	}
	
	else if( !Settings::ESP::enabled ){
		modelRenderVMT->GetOriginalMethod<DrawModelExecuteFn>(21)(thisptr, context, state, pInfo, pCustomBoneToWorld);
		modelRender->ForcedMaterialOverride(nullptr);
	}
	

	if (!Settings::ScreenshotCleaner::enabled || !engine->IsTakingScreenshot())
	{
		ESP::DrawModelExecute();
	}
}
