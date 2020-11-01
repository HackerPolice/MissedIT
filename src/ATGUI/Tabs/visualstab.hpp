#pragma once

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../../Hacks/tracereffect.h"
#include "../../Hacks/materialconfig.h"
#include "../../Utils/ColorPickerButton.h"

namespace Visuals
{
	void RenderTab();

	void Visibility();
	void Items();
	void PlayerDetails();
	void DangerZone();
	void Supportive();
	void Others();
}