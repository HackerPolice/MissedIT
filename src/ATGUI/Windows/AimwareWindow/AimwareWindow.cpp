#include "AimwareWindow.hpp"
#include "../main.h"
#include "../../interfaces.h"
// #include "../Tabs/antiaimtab.hpp"
#include <vector>

static int tabs = 0;
static int subtabs = 0;

enum {
    AimbotTab,
    AntiAimTab,
    VisualTab,
    SkinsModelsTab,
    MiscTab,
    Config,
};

enum {
    LegitbotTab,
    RagebotTab,
    TriggerbotTab,
};

/**
 * @brief If you add a Tab name make sure you add submenu for that atleast with one name 
 * Otherwise the game will crash
 * After that you have to add that submenu into allsubtabs sequentially
 */
const char* TabsNames[] = {"Aimbot", "AntiAIm", "Visuals", "Skins/Models", "Misc", "Config"};

// Sub Menus easy to create You just have to create the sub menu an let the program do it's job
// Everything other things will be handled
std::initializer_list<const char *> subtabAimbot = {"LegitBot", "Ragebot", "Triggerbot"};
std::initializer_list<const char *> subtabAntiAim = { "HvH", "FakeLag" };
std::initializer_list<const char *> subtabVisuals = { "Players", "Radar", "Hitmarker", "Others" };
std::initializer_list<const char *> subtabSkinsModels = {"Skins", "Models", "Player" };
std::initializer_list<const char *> subtabMisc = { "BunnyHop", "Customize", "Spammer", "Others", "Unload/Eject"};
std::initializer_list<const char *> subtabConfig = { "Overview", "Add", "Refresh", "Save", "Remove" };

// If you add a new submenu make sure you addd that sequentially :)
std::vector<std::initializer_list<const char *>> allsubtabs{
    subtabAimbot,
    subtabAntiAim,
    subtabVisuals,
    subtabSkinsModels,
    subtabMisc,
    subtabConfig,
}; 

static ImVec2 pos;
ImDrawList *draw;

/**
 * @brief A Ui looks similar to Aimware V5 Thanks to aoshax
 * Post Link : https://yougame.biz/threads/182265/
 * User Profile Link : https://yougame.biz/members/211832/
 */

static void BgDecorations()
{
	ImGuiIO& io = ImGui::GetIO();
	
	auto info = io.FontDefault;
    pos = ImGui::GetWindowPos();
    draw = ImGui::GetWindowDrawList();

	// Background
    draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 960, pos.y + 645), ImColor(8, 8, 8, 200), 0);
	// Header
    draw->AddRectFilled(ImVec2(pos.x, pos.y + 50), ImVec2(pos.x + 960, pos.y + 52), ImColor(220, 60, 40));
    // Footer Border
    draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + 960, pos.y + 50), ImColor(187, 37, 38), 0, 3);
    // Footer
    draw->AddRectFilled(ImVec2(pos.x, pos.y + 615), ImVec2(pos.x + 960, pos.y + 645), ImColor(187, 37, 38), 0, 4 | 8);
    //
    //
    draw->AddText(info, 13.f, ImVec2(pos.x + 12, pos.y + 625), ImColor(255, 255, 255), "1.6.5 LTE");
    draw->AddText(info, 13.f, ImVec2(pos.x + 100, pos.y + 625), ImColor(255, 255, 255), "Counter-Strike: Global Offensive");
    draw->AddText(info, 13.f, ImVec2(pos.x + 860, pos.y + 625), ImColor(255, 255, 255), "MissedIT.cc");
    //
    // Side Panel Bg
	draw->AddRectFilled(ImVec2(pos.x, pos.y + 55), ImVec2(pos.x + 160, pos.y + 615), ImColor(0, 0, 0, 150));
    ImGui::SetCursorPos(ImVec2(10, 10));
    //
    // draw->AddRectFilled(ImVec2(pos.x + 595, pos.y + 15), ImVec2(pos.x + 785, pos.y + 35), ImColor(220, 220, 220), 5);
    // draw->AddText(info, 13.f, ImVec2(pos.x + 600, pos.y + 18), ImColor(136, 136, 134), u8"���� search features �� ���");
    //
}

// Upper Tabs
static void Tabs(){

    static int tab_sizes = IM_ARRAYSIZE(TabsNames);
    static int tab_gaps = 120;
    static int tabY = 10;
    static int tabX;
    tabX = 60;
    for (int i = 0; i < tab_sizes; i++){
        ImGui::SetCursorPos(ImVec2(tabX, tabY));
        if (ImGui::Tab(TabsNames[i], i == tabs)){
            tabs = i;
            subtabs = 0;
        }
            
        tabX += tab_gaps;
    }
    // ImGui::SetCursorPos(ImVec2(165, 10));
    // if (ImGui::Tab("Aimbot", 0 == tabs))
    //     tabs = 1;
    // ImGui::SetCursorPos(ImVec2(270, 10));
    // if (ImGui::Tab("TriggerBot", 1 == tabs))
    //     tabs = 2;
    // ImGui::SetCursorPos(ImVec2(375, 10));
    // if (ImGui::Tab("AntiAIm", 2 == tabs))
    //     tabs = 3;
    // ImGui::SetCursorPos(ImVec2(480, 10));
    // if (ImGui::Tab("Visuals", 3 == tabs))
    //     tabs = 4;
    // ImGui::SetCursorPos(ImVec2(60, 10));
    // if (ImGui::Tab("Skins/Models", 4 == tabs))
    //     tabs = 0;
	// ImGui::SetCursorPos(ImVec2(60, 10));
	// if (ImGui::Tab("Misc", 5 == tabs))
    //     tabs = 0;
}

// Side Tabs
static void SubTabs(){

    // I did not find any better solution for now so Duck Duck Go

    static int tab_gaps = 40;
    static int tabY;
    static int tabX = 0;
    tabY = 53;
    static int subtabs_size = allsubtabs.size();
    for ( int i = 0; i < subtabs_size; i++){
        if ( i == tabs){
            int j = 0;
            for (auto _subtabname : allsubtabs[i]){
                ImGui::SetCursorPos(ImVec2(tabX, tabY));
                if (ImGui::Sub(_subtabname, j == subtabs))
                    subtabs = j;
                j++;
                tabY += tab_gaps;
            }
            break;
        }
    }
    
	// ImGui::SetCursorPos(ImVec2(0, 53));
    // if (ImGui::Sub("Aimbot", 0 == subtabs))
    //     subtabs = 0;
    // ImGui::SetCursorPos(ImVec2(0, 93));
    // if (ImGui::Sub("Triggerbot", 1 == subtabs))
    //     subtabs = 1;
    // ImGui::SetCursorPos(ImVec2(0, 133));
    // if (ImGui::Sub("Weapon", 2 == subtabs))
    //     subtabs = 2;
    // ImGui::SetCursorPos(ImVec2(0, 173));
    // if (ImGui::Sub("Other", 3 == subtabs))
    //     subtabs = 3;
    // ImGui::SetCursorPos(ImVec2(0, 213));
    // if (ImGui::Sub("Semirage", 4 == subtabs))
    //     subtabs = 4;
}

static void AimBotTab(){
    switch (subtabs)
    {
    case LegitbotTab: // Legit bot tab
        Legitbot::RenderAimware(pos, draw, subtabs);
        break;
    case RagebotTab:
        Ragebot::RenderAimware(pos, draw, subtabs);
        break;
    default:
        Triggerbot::RenderAimware(pos, draw, subtabs);
        break;
    }
        
}

/**
 * @brief
 * Mainly Responsible for start the rendering to aimware style menu
 */
void AimwareWindow::Render(){
    ImGui::SetNextWindowBgAlpha( 0.0f );
	if (ImGui::Begin(XORSTR("##MissedIt-AimWare-Menu"), &Main::showWindow, ImGuiWindowFlags_NoCollapse /*| ImGuiWindowFlags_NoMove*/ | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiConfigFlags_NoMouseCursorChange ) )
	{
		BgDecorations();
		Tabs();
		SubTabs();
        
        // cvar->ConsoleDPrintf(XORSTR("Selected tab : %d\n"), tabs);
        switch (tabs)
        {
        case AimbotTab:
            AimBotTab();
            break;
        case AntiAimTab:
             HvH::RenderAimware(pos, draw, subtabs);
            break;
        case VisualTab:
            Visuals::RenderAimware(pos, draw, subtabs);
            break;
        case SkinsModelsTab:
            SkinsAndModel::RenderAimware(pos, draw, subtabs);
            break;
        case MiscTab:
            Misc::RenderAimware(pos, draw, subtabs);
            break;
        case Config:
            Configs::RenderAimware(pos, draw, subtabs);
            subtabs = 0;
        default:
            break;
        }
    
		ImGui::End();
	}
}