#pragma once

#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../tooltip.h"

namespace Triggerbot
{
	void RenderTab();

	void RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex);

	void KeyBind();

	void Delay();

	void AutoKnife();

	void Filters();
}
