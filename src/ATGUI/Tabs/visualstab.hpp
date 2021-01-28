#pragma once

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../../Hacks/tracereffect.h"
#include "../../Hacks/materialconfig.h"
#include "../../Utils/ColorPickerButton.h"
#include "other.hpp"

namespace Visuals
{
	void RenderTab();
	void RenderAimware(ImVec2 &pos, ImDrawList * draw, int sideTabIndex);

	void Visibility();
	void Items();
	void PlayerDetails();
	void DangerZone();
	void Supportive();
	void Others();
}