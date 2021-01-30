#include "tooltip.h"
#include "../ImGUI/imgui.h"

void ToolTip::Show(const char *tooltip, bool IsItemHovered)
{
	if (!IsItemHovered) {
		return;
	}
	// auto massage = tooltip;
	ImGui::SetNextWindowSize(ImVec2(), ImGuiSetCond_Always);
	ImGui::SetTooltip("%s", tooltip);
}
