#pragma once

#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui.h"
#include "../../settings.h"
#include "skinstab.h"
#include "modelstab.h"

namespace SkinsAndModel
{
	void RenderTab();

	void RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex);
}
