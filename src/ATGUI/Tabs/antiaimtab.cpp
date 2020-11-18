#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "antiaimtab.h"

#include "../../interfaces.h"
#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../../Hacks/valvedscheck.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../tooltip.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

void HvH::AntiAim(){

    ImGui::PushItemWidth(-1);
    ImGui::Columns();
    {
        ImGui::SliderFloat(XORSTR("##offset"), &Settings::AntiAim::offset, 0, 360, XORSTR(" Real Offset : %0.0f"));
        ToolTip::Show("Ajust Your Player angle in Y axis Usefull in HVH", ImGui::IsItemHovered());
    }
            
    ImGui::Columns();
    {
        ImGui::SliderFloat(XORSTR("##Jitter"), &Settings::AntiAim::JitterAmmount, 0, 100, XORSTR(" Jitter : %0.0f"));
        ToolTip::Show("Boop Boop Shaking head enjoy but only fake will shake", ImGui::IsItemHovered());
    }
    ImGui::PopItemWidth();
}

void HvH::RageFeatures(){
    ImGui::Columns();
    {
        ImGui::Checkbox(XORSTR("##pitchDown"), &Settings::AntiAim::PitchDown);
        ImGui::SameLine();
        ImGui::Text(XORSTR("PitchDown"));
        ToolTip::Show("Make Your Player Look Down", ImGui::IsItemHovered());
    }
    ImGui::Columns();
    {
        ImGui::Checkbox(XORSTR("##FakeJitter"), &Settings::AntiAim::JitterFake);
        ImGui::SameLine();
        ImGui::Text(XORSTR("Fake Jitter"));
        ToolTip::Show("It Will Jitter Fake and can break some Cheat resolvers but make sure you turn on jitter too", ImGui::IsItemHovered());
    }
    ImGui::Columns();
    {
        ImGui::Checkbox(XORSTR("##AtTheTarget"), &Settings::AntiAim::atTheTarget);
        ImGui::SameLine();
        ImGui::Text(XORSTR("At The Target"));
        ToolTip::Show("Automatically Angle Your player Towords your Enemy", ImGui::IsItemHovered());
    }
    ImGui::Columns();
    {
        ImGui::Checkbox(XORSTR("##AutoDirection"), &Settings::AntiAim::autoDirection);
        ImGui::SameLine();
        ImGui::Text(XORSTR("Auto Direction"));
        ToolTip::Show("AutoMatically Adjust Your Head According to your Enemy", ImGui::IsItemHovered());
    }

    ImGui::Columns(); // Invert On Shoot
    {
        ImGui::Checkbox(XORSTR("##InvertOnShoot"), &Settings::AntiAim::InvertOnShoot);
        ImGui::SameLine();
        ImGui::Text(XORSTR("Invert On Shoot"));
        ToolTip::Show("Will Invert Your AntiAim After every shoot", ImGui::IsItemHovered());
    }
}

void HvH::ManualAntiAim(){
    ImGui::Checkbox(XORSTR("##ManualAntiAim"), &Settings::AntiAim::ManualAntiAim::Enable);
    ImGui::SameLine();
    ImGui::Text(XORSTR("Enable"));

    ImGui::Text("Back");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::backButton);
    ImGui::PopItemWidth();

    ImGui::Text("Right");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::RightButton);
    ImGui::PopItemWidth();

    ImGui::Text("Left ");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    UI::KeyBindButton(&Settings::AntiAim::ManualAntiAim::LeftButton);
    ImGui::PopItemWidth();

}

void HvH::Others(){

    ImGui::Columns();
    {
        ImGui::Checkbox(XORSTR("ShowReal"), &Settings::AntiAim::ShowReal);
        ToolTip::Show("A arrow will show the side of the real", ImGui::IsItemHovered());
    }
    ImGui::PushItemWidth(-1);

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

void HvH::FakeLag(){
    ImGui::PushItemWidth(-1);
            
    ImGui::Checkbox(XORSTR("Fake Lag"), &Settings::FakeLag::enabled);
    ImGui::SameLine();
    ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 50, XORSTR("Amount: %0.f"));
	ImGui::Checkbox(XORSTR("Adaptive Fake Lag"), &Settings::FakeLag::adaptive);
            
    ImGui::PopItemWidth();
}

void HvH::RenderTab()
{

   ImGui::Checkbox(XORSTR("Enable"), &Settings::AntiAim::Enabled);

    ImGui::Columns(2, nullptr, false);
    {
        ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), false);
        {
            
            ImGui::BeginGroupPanel(XORSTR("AntiAim"));
            {
                AntiAim();
            }ImGui::EndGroupPanel();
           
            ImGui::BeginGroupPanel(XORSTR("Rage"));
            {
                RageFeatures();
            }ImGui::EndGroupPanel();   
            
            ImGui::BeginGroupPanel(XORSTR("Manaul AntiAim"));
            {
                ManualAntiAim();
            }ImGui::EndGroupPanel();

            
        }
        ImGui::EndChild();
    }
    ImGui::NextColumn();
    {
        ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), false);
        {
            ImGui::BeginGroupPanel(XORSTR("Others"));
            {
                Others();
            }ImGui::EndGroupPanel();

            ImGui::BeginGroupPanel(XORSTR("FakeLag"));
            {
                FakeLag();
            }ImGui::EndGroupPanel();
        }
        ImGui::EndChild();
    }
}
