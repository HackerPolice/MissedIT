#include "other.hpp"

#include "../../interfaces.h"

#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"

static const char *Sounds[] = {"None", "SpongeBob", "Half life", "Half life 2", "Half life 3", "Half life 4",
                               "BB Gun Bell", "Dopamine", "Wub", "Pedo Yes!", "Meme", "Error", "Orchestral"};
static const char *TeamColorTypes[] = {"Absolute", "Relative"};

void Other::RadarOptions()
{

	ImGui::CheckboxFill(XORSTR("##RadarEnable"), &Settings::Radar::enabled);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Enable"));
	ImGui::CheckboxFill(XORSTR("In-game Radar"), &Settings::Radar::InGame::enabled);

	ImGui::PushItemWidth(-1);
	ImGui::Combo(XORSTR("##RADARTEAMCOLTYPE"), (int *) &Settings::Radar::teamColorType, TeamColorTypes,
	             IM_ARRAYSIZE(TeamColorTypes));
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(-1);
	ImGui::SliderFloat(XORSTR("##RADARZOOM"), &Settings::Radar::zoom, 0.f, 100.f, XORSTR("Zoom: %0.f%%"));
	ImGui::SliderFloat(XORSTR("##RADARICONSSCALE"), &Settings::Radar::iconsScale, 2, 16, XORSTR("Icons Scale: %0.1f"));
	ImGui::PopItemWidth();

	ImGui::BeginGroupPanel(XORSTR("Filters"));
	{
		ImGui::Selectable(XORSTR("Enemies"), &Settings::Radar::enemies);
		ImGui::Selectable(XORSTR("Bomb"), &Settings::Radar::bomb);
		ImGui::Selectable(XORSTR("Legit"), &Settings::Radar::legit);
		ImGui::Selectable(XORSTR("Allies"), &Settings::Radar::allies);
		ImGui::Selectable(XORSTR("Defuser"), &Settings::Radar::defuser);
		ImGui::Selectable(XORSTR("Visibility Check"), &Settings::Radar::visibilityCheck);
		ImGui::Selectable(XORSTR("Smoke Check"), &Settings::Radar::smokeCheck);
	}
	ImGui::EndGroupPanel();

}

void Other::Hitmarkers()
{

	ImGui::CheckboxFill(XORSTR("Hitmarkers"), &Settings::ESP::Hitmarker::enabled);
	ImGui::CheckboxFill(XORSTR("Damage##HITMARKERS"), &Settings::ESP::Hitmarker::Damage::enabled);
	ImGui::CheckboxFill(XORSTR("Sounds##HITMARKERS"), &Settings::ESP::Hitmarker::Sounds::enabled);

	if (Settings::ESP::Hitmarker::Sounds::enabled) {
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR ("##HITMARKERCOMBO"), (int *) &Settings::ESP::Hitmarker::Sounds::sound, Sounds,
		             IM_ARRAYSIZE(Sounds));
		ImGui::PopItemWidth();
	}

	ImGui::PushItemWidth(-1);
	ImGui::SliderInt(XORSTR("##HITMARKERDUR"), &Settings::ESP::Hitmarker::duration, 250, 3000, XORSTR("Timeout: %0.f"));
	ImGui::SliderInt(XORSTR("##HITMARKERSIZE"), &Settings::ESP::Hitmarker::size, 1, 32, XORSTR("Size: %0.f"));
	ImGui::SliderInt(XORSTR("##HITMARKERGAP"), &Settings::ESP::Hitmarker::innerGap, 1, 16, XORSTR("Gap: %0.f"));
	ImGui::PopItemWidth();

	ImGui::BeginGroupPanel(XORSTR("Filters"));
	{
		ImGui::PushItemWidth(-1);
		ImGui::Selectable(XORSTR("Enemies##HITMARKERS"), &Settings::ESP::Hitmarker::enemies);
		ImGui::Selectable(XORSTR("Allies##HITMARKERS"), &Settings::ESP::Hitmarker::allies);
		ImGui::PopItemWidth();
	}
	ImGui::EndGroupPanel();

}

void Other::Render()
{

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("##RadarOptions"), ImVec2(0, 0), false);
		{
			ImGui::BeginGroupPanel(XORSTR("Radar"));
			{
				RadarOptions();
			}
			ImGui::EndGroupPanel();

		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("##Hitmarker"), ImVec2(0, 0), false);
		{
			ImGui::BeginGroupPanel(XORSTR("Hitmarker"));
			{
				Hitmarkers();
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::EndColumns();

}
