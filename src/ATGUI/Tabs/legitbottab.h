#pragma once

#include "../tooltip.h"
#include "../../../ImGUI/imgui.h"
#include "../../../ImGUI/imgui_internal.h"

namespace Legitbot
{
	void RenderTab();

	void RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex);
}
