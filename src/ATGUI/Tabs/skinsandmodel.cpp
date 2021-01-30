#include "skinsandmodel.h"

const char *skinORmodel[] = {"Skins", "Models"};

void SkinsAndModel::RenderTab()
{
	ImGui::PushItemWidth(-1);
	ImGui::Combo(XORSTR("##SkinsOrModels"), (int *) &Settings::SkinOrModel::skin_or_model, skinORmodel,
	             IM_ARRAYSIZE(skinORmodel));
	ImGui::PopItemWidth();

	switch (Settings::SkinOrModel::skin_or_model) {
		case SkinAndModel::Skins:
			Skins::RenderTab();
			break;
		case SkinAndModel::Model:
			Models::RenderTab();
			break;
	}

}

void SkinsAndModel::RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{

	ImGui::SetCursorPos(ImVec2(180, 65));
	ImGui::BeginGroup();
	{
		if (sideTabIndex == (int) SkinAndModel::Skins) {
			Skins::RenderTab();
		} else if (sideTabIndex == (int) SkinAndModel::Model) {
			Models::RenderTab();
		} else {

		}
	}
	ImGui::EndGroup();

}
