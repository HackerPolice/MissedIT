#include "skinsandmodel.h"

const char *skinORmodel[] = {"Skins", "Models"};


static const char *modelNames[] = {
		"Ricksaw",
		"Operator | FBI SWAT",
		"Ava",
		"Seal Team 6 Soldier | NSWC SEAL",
		"3rd Commando Company | KSK",
		"B Squadron Officer | SAS",
		"Chem-Haz Specialist | SWAT",
		"Bio-Haz Specialist | SWAT",
		"Enforcer | Phoenix",
		"Soldier | Phoenix",
		"Ground Rebel  | Elite Crew",
		"Street Soldier | Phoenix",
		"Dragomir | Sabre Footsoldier",
		"Markus Delrow | FBI",
		"Buckshot | NSWC SEAL",
		"John 'Van Healen' Kask | SWAT",
		"Sergeant Bombson | SWAT",
		"'Blueberries' Buckshot | NSWC SEAL",
		"NONE"
};

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
			ImGui::BeginGroupPanel(XORSTR("Player Model Changer"));
			{
				ImGui::Combo(XORSTR("##SelectPlayerModel"), (int *) &Settings::Skinchanger::Models::playerModel, modelNames, IM_ARRAYSIZE(modelNames));
			}
			ImGui::EndGroupPanel();
		}
	}
	ImGui::EndGroup();

}
