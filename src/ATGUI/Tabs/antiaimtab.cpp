#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "antiaimtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"


void HvH::RenderTab()
{
    
    ImGui::Spacing();

    ImGui::Columns(2, nullptr, false);
    {
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), false);
        {
            ImGui::PushItemWidth(-1);
            if ( Settings::AntiAim::offset == 0 || Settings::AntiAim::offset == 360) // amtiaim off
            {
                ImGui::SliderFloat(XORSTR("##offset"), &Settings::AntiAim::offset, 0, 360, XORSTR(" Increase or Decrease to turn on antiaim"));
            }
            else 
            {
                ImGui::SliderFloat(XORSTR("##offset"), &Settings::AntiAim::offset, 0, 360, XORSTR(" Real Offset : %0.0f"));
            }
            if ( Settings::AntiAim::fakeAmmount == 0) // amtiaim off
            {
                ImGui::SliderFloat(XORSTR("##FakeAmmount"), &Settings::AntiAim::fakeAmmount, 0, 100, XORSTR(" Increase to turn on antiaim"));
            }
            else 
            {
                ImGui::SliderFloat(XORSTR("##FakeAmmount"), &Settings::AntiAim::fakeAmmount, 0, 100, XORSTR(" Fake Ammount : %0.0f"));
            }

            ImGui::Columns();
            {
                ImGui::Checkbox(XORSTR("##pitchDown"), &Settings::AntiAim::PitchDown);
                ImGui::SameLine();
                ImGui::Text(XORSTR("PitchDown"));
            }
            ImGui::Columns();
            {
                ImGui::Checkbox(XORSTR("##AtTheTarget"), &Settings::AntiAim::atTheTarget);
                ImGui::SameLine();
                ImGui::Text(XORSTR("At The Target"));
            }
            ImGui::Columns();
            {
                ImGui::Checkbox(XORSTR("##AutoDirection"), &Settings::AntiAim::autoDirection);
                ImGui::SameLine();
                ImGui::Text(XORSTR("Auto Direction"));
            }
            ImGui::PopItemWidth();
        // }
        // ImGui::EndChild();

        // ImGui::BeginChild(XORSTR("ManualAntiAim"), ImVec2(0,0), true);{
            ImGui::Checkbox(XORSTR("##Manual Anti Aim"), &Settings::AntiAim::ManualAntiAim::Enable);
            ImGui::SameLine();
            ImGui::Text(XORSTR("Manuan AntiAim"));
            ImGui::Text("Align Right");
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::RightButton);
            ImGui::PopItemWidth();

            // For player Move back
            ImGui::Text("Align Back ");
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::backButton);
            ImGui::PopItemWidth();

            //For player to move left
            ImGui::Text("Align Left ");
            ImGui::SameLine();
            ImGui::PushItemWidth(-1);
            UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::LeftButton);
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), false);
        {
            ImGui::PushItemWidth(-1);
            ImGui::Checkbox(XORSTR("Angle Indicator"), &Settings::AngleIndicator::enabled);
            ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
            ImGui::SameLine();
            ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 100, XORSTR("Amount: %0.f"));
			ImGui::Checkbox(XORSTR("Adaptive Fake Lag"), &Settings::FakeLag::adaptive);
            
            ImGui::Checkbox(XORSTR("FakeDuck"), &Settings::AntiAim::FakeDuck::enabled);
            ImGui::SameLine(); 
            UI::KeyBindButton(&Settings::AntiAim::FakeDuck::fakeDuckKey);

            ImGui::Checkbox(XORSTR("FakeWalk"), &Settings::AntiAim::FakeWalk::enabled);
            ImGui::SameLine();
            ImGui::SliderFloat(XORSTR("##FakeWalkSPeed"), &Settings::AntiAim::FakeWalk::Speed, 0, 100, XORSTR("Amount: %0.f"));
            if ( Settings::AntiAim::FakeWalk::enabled ) 
                UI::KeyBindButton(&Settings::AntiAim::FakeWalk::Key);

            ImGui::Checkbox(XORSTR("SlowWalk"), &Settings::AntiAim::SlowWalk::enabled);
            ImGui::SameLine();
            ImGui::SliderFloat(XORSTR("##SlowWalkSPeed"), &Settings::AntiAim::SlowWalk::Speed, 0, 100, XORSTR("Amount: %0.f"));
            if ( Settings::AntiAim::SlowWalk::enabled ) 
                UI::KeyBindButton(&Settings::AntiAim::SlowWalk::Key);
            
            ImGui::PopItemWidth();
        }
        ImGui::EndChild();
    }
}
