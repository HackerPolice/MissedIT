#include "other.hpp"

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
#include "../../Hacks/valvedscheck.h"
#include "../Windows/playerlist.h"


static void RaderOptions(){

	const char* TeamColorTypes[] = { "Absolute", "Relative" };
	
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Radar"), &Settings::Radar::enabled);
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR("##RADARTEAMCOLTYPE"), (int*)& Settings::Radar::teamColorType, TeamColorTypes, IM_ARRAYSIZE(TeamColorTypes));
		ImGui::PopItemWidth();
		ImGui::Checkbox(XORSTR("Enemies"), &Settings::Radar::enemies);
		ImGui::Checkbox(XORSTR("Bomb"), &Settings::Radar::bomb);
		ImGui::Checkbox(XORSTR("Legit"), &Settings::Radar::legit);
		ImGui::Checkbox(XORSTR("In-game Radar"), &Settings::Radar::InGame::enabled);
	}
	ImGui::NextColumn();
	{
		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat(XORSTR("##RADARZOOM"), &Settings::Radar::zoom, 0.f, 100.f, XORSTR("Zoom: %0.f"));
		ImGui::SliderFloat(XORSTR("##RADARICONSSCALE"), &Settings::Radar::iconsScale, 2, 16, XORSTR("Icons Scale: %0.1f"));
		ImGui::PopItemWidth();
		ImGui::Checkbox(XORSTR("Allies"), &Settings::Radar::allies);
		ImGui::Checkbox(XORSTR("Defuser"), &Settings::Radar::defuser);
		ImGui::Checkbox(XORSTR("Visibility Check"), &Settings::Radar::visibilityCheck);
		ImGui::Checkbox(XORSTR("Smoke Check"), &Settings::Radar::smokeCheck);
	}
    ImGui::EndColumns();

}

static void Hitmarkers(){

	const char* Sounds[] = { "None", "SpongeBob", "Half life", "Half life 2", "Half life 3", "Half life 4", "BB Gun Bell", "Dopamine", "Wub", "Pedo Yes!", "Meme", "Error", "Orchestral" };
	
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::Checkbox(XORSTR("Hitmarkers"), &Settings::ESP::Hitmarker::enabled);

		ImGui::Checkbox(XORSTR("Enemies##HITMARKERS"), &Settings::ESP::Hitmarker::enemies);
		ImGui::Checkbox(XORSTR("Allies##HITMARKERS"), &Settings::ESP::Hitmarker::allies);
        ImGui::Checkbox(XORSTR("Sounds##HITMARKERS"), &Settings::ESP::Hitmarker::Sounds::enabled);
        ImGui::Checkbox(XORSTR("Damage##HITMARKERS"), &Settings::ESP::Hitmarker::Damage::enabled);
    }
	ImGui::NextColumn();
	{
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt(XORSTR("##HITMARKERDUR"), &Settings::ESP::Hitmarker::duration, 250, 3000, XORSTR("Timeout: %0.f"));
    	ImGui::SliderInt(XORSTR("##HITMARKERSIZE"), &Settings::ESP::Hitmarker::size, 1, 32, XORSTR("Size: %0.f"));
		ImGui::SliderInt(XORSTR("##HITMARKERGAP"), &Settings::ESP::Hitmarker::innerGap, 1, 16, XORSTR("Gap: %0.f"));
        ImGui::Combo( XORSTR ( "Sounds##HITMARKERCOMBO" ), ( int* ) &Settings::ESP::Hitmarker::Sounds::sound, Sounds, IM_ARRAYSIZE( Sounds ) );
       ImGui::PopItemWidth();
	}
}
void Other::Render(){

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("##RaderOptions"), ImVec2(0,0), true);
		{
			RaderOptions();
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("##Hitmarker"), ImVec2(0,0), true);
		{
			Hitmarkers();
		}
		ImGui::EndChild();
	}
	ImGui::EndColumns();
	
	
	// ImGui::Columns(1);
	// ImGui::Separator();

	// ;
}