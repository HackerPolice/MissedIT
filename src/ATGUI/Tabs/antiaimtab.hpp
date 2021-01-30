#pragma once

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/serverinfo.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../tooltip.h"

namespace HvH
{
	void RenderTab();

	void RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex);

	void AntiAim();

	void RageFeatures();

	void ManualAntiAim();

	void Others();

	void FakeLag();

	void LBYBreak();

	void Jitter();
}
