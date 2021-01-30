#pragma once

#include <sys/stat.h>
#include <sstream>

#include "../../config.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../imgui.h"
#include "../atgui.h"
#include "../tooltip.h"

namespace Configs
{
	void RenderTab();

	void RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex);
}
