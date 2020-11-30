#include "main.h"

#include "../../settings.h"
#include "../../ImGUI/imgui_internal.h"
#include "../../Utils/xorstring.h"

#include "../Tabs/legitbottab.h"
#include "../Tabs/ragebottab.h"
#include "../Tabs/antiaimtab.hpp"
#include "../Tabs/misctab.h"
#include "../Tabs/triggerbottab.h"
#include "../Tabs/visualstab.hpp"
#include "../Tabs/skinstab.h"
#include "../Tabs/modelstab.h"
#include "../Tabs/skinsandmodel.h"
#include "../Tabs/Colors.hpp"
#include "../Tabs/other.hpp"

#include "colors.h"
#include "configs.h"

bool Main::showWindow = true;

void Main::RenderWindow()
{
	if (!Main::showWindow)
	{
		Settings::UI::Windows::Main::open = false;
		return;
	} 

	ImVec2 size =  ImGui::GetWindowSize();
	size = ImVec2( (size.x - Settings::UI::Windows::Main::sizeX)/ 2, (size.y - Settings::UI::Windows::Main::sizeY) / 2);
	if( Settings::UI::Windows::Main::reload )
	{
		ImGui::SetNextWindowPos(ImVec2(size.x, size.y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiCond_Once);
		Main::showWindow = Settings::UI::Windows::Main::open;
		Settings::UI::Windows::Main::reload = false;
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(size.x, size.y), ImGuiCond_Once);
		ImGui::SetNextWindowSize(ImVec2(Settings::UI::Windows::Main::sizeX, Settings::UI::Windows::Main::sizeY), ImGuiCond_Once);
	}

	static int page = 0;

	ImVec2 temp = ImGui::GetWindowSize();
	
	if (ImGui::Begin(XORSTR("##MissedIt"), &Main::showWindow, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoScrollbar /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoTitleBar | ImGuiConfigFlags_NoMouseCursorChange ) )
	{
		Settings::UI::Windows::Main::open = true;

		Settings::UI::Windows::Main::sizeX = (int)temp.x;
		Settings::UI::Windows::Main::sizeY = (int)temp.y;
		temp = ImGui::GetWindowPos();
		Settings::UI::Windows::Main::posX = (int)temp.x;
		Settings::UI::Windows::Main::posY = (int)temp.y;
		const char* tabs[] = {
				"Legit Bot",
				"Rage Bot",
				"Trigger Bot",
				"Anti Aim",
				"Visuals",
				"Colors",
				"Skin/Model",
				"Misc",
				"Others",
				"Config",
		};

		ImGui::Columns(2, nullptr, false);
		{
			float ButtonsXSize = ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs)-9;
			ImGui::SetColumnOffset(1, ButtonsXSize); 
			for (int i = 0; i < 10; i++)
			{
				int distance = i == page ? 0 : i > page ? i - page : page - i;
				
				if (i <= 5){
					ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
						Settings::UI::mainColor.Color().Value.x - (distance * 0.035f),
						Settings::UI::mainColor.Color().Value.y - (distance * 0.035f),
						Settings::UI::mainColor.Color().Value.z - (distance * 0.035f),
						Settings::UI::mainColor.Color().Value.w
					);
				}
				
				ImGui::Spacing();
				if (ImGui::Button(tabs[i], ImVec2( ButtonsXSize, 0)))
					page = i;

				if (i == 5) ImGui::Dummy(ImVec2(-1,80));
				ImGui::GetStyle().Colors[ImGuiCol_Button] =Settings::UI::mainColor.Color().Value;
				
			}
		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
			{
				switch (page)
				{
				case 0:
					Legitbot::RenderTab();
					break;
				case 1:
					RagebotTab::RenderTab();
					break;
				case 2:
					Triggerbot::RenderTab();
					break;
				case 3:
					HvH::RenderTab();
					break;
				case 4:
					Visuals::RenderTab();
					break;
				case 5:
					Colors::RenderTab();
					break;
				case 6:
					SkinsAndModel::RenderTab();
					break;
				case 7:
					Misc::RenderTab();
					break;
				case 8:
					Other::Render();
					break;
				case 9:
					Configs::RenderTab();
					break;
				}
			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();
		ImGui::End();
	}
}