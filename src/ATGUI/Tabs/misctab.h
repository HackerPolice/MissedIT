#pragma once

#include <sys/stat.h>
#include <sstream>

#include "../../config.h"
#include "../../interfaces.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../imgui.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"

#include "../../Hacks/namechanger.h"
#include "../../Hacks/namestealer.h"
#include "../../Hacks/grenadehelper.h"
#include "../../Hacks/clantagchanger.h"
#include "../../Hacks/serverinfo.h"
#include "../Windows/playerlist.h"
#include "../../MissedIt.h"
namespace Misc
{
	void RenderTab();
	void RenderAimware(ImVec2 &pos, ImDrawList * draw, int sideTabIndex);
}
