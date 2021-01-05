#include "triggerbottab.h"

#include "../../Utils/xorstring.h"
#include "../../settings.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void Triggerbot::KeyBind()
{

	ImGui::Checkbox(XORSTR("Active On Key"), &Settings::Triggerbot::OnKey::enable);
	ImGui::PushItemWidth(-1);
	UI::KeyBindButton(&Settings::Triggerbot::OnKey::key);
	ImGui::PopItemWidth();
}

void Triggerbot::Delay()
{

	ImGui::Checkbox(XORSTR("Randome Delay"), &Settings::Triggerbot::RandomDelay::enabled);
	if (Settings::Triggerbot::RandomDelay::lastRoll != 0) {
		ImGui::Text(XORSTR("Last delay: %dms"), Settings::Triggerbot::RandomDelay::lastRoll);
	}

	ImGui::PushItemWidth(-1);
	ImGui::SliderInt(XORSTR("##TRIGGERRANDOMLOW"), &Settings::Triggerbot::RandomDelay::lowBound, 5, 220,
	                 XORSTR("Minimum ms : %0.2f"));
	if (Settings::Triggerbot::RandomDelay::lowBound >= Settings::Triggerbot::RandomDelay::highBound) {
		Settings::Triggerbot::RandomDelay::highBound = Settings::Triggerbot::RandomDelay::lowBound + 1;
	}

	ImGui::SliderInt(XORSTR("##TRIGGERRANDOMHIGH"), &Settings::Triggerbot::RandomDelay::highBound,
	                 (Settings::Triggerbot::RandomDelay::lowBound + 1), 225, XORSTR("Maximum ms : %0.2f"));
	ImGui::PopItemWidth();
}

void Triggerbot::AutoKnife()
{

	ImGui::Checkbox(XORSTR("Auto Knife"), &Settings::AutoKnife::enabled);
	ImGui::Checkbox(XORSTR("On Key"), &Settings::AutoKnife::onKey);

	ImGui::Checkbox(XORSTR("Enemies"), &Settings::AutoKnife::Filters::enemies);
	ImGui::Checkbox(XORSTR("Allies"), &Settings::AutoKnife::Filters::allies);

}

void Triggerbot::Filters()
{

	ImGui::Selectable(XORSTR("Enemies"), &Settings::Triggerbot::Filters::enemies);
	ImGui::Selectable(XORSTR("Walls"), &Settings::Triggerbot::Filters::walls);
	ImGui::Selectable(XORSTR("Head"), &Settings::Triggerbot::Filters::head);
	ImGui::Selectable(XORSTR("Chest"), &Settings::Triggerbot::Filters::chest);
	ImGui::Selectable(XORSTR("Legs"), &Settings::Triggerbot::Filters::legs);

	ImGui::Selectable(XORSTR("Allies"), &Settings::Triggerbot::Filters::allies);
	ImGui::Selectable(XORSTR("Smoke check"), &Settings::Triggerbot::Filters::smokeCheck);
	ImGui::Selectable(XORSTR("Flash check"), &Settings::Triggerbot::Filters::flashCheck);
	ImGui::Selectable(XORSTR("Stomach"), &Settings::Triggerbot::Filters::stomach);
	ImGui::Selectable(XORSTR("Arms"), &Settings::Triggerbot::Filters::arms);

}

void Triggerbot::RenderTab()
{
	ImGui::Checkbox(XORSTR("Enabled"), &Settings::Triggerbot::enabled);
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("TRIG1"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel("OnKey");
			{
				KeyBind();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel("Delay");
			{
				Delay();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel("Auto Knife or Zues");
			{
				AutoKnife();
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("TRIG2"), ImVec2(0, 0), false);
		{
			ImGui::BeginGroupPanel("Filters");
			{
				Filters();
			}
			ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
}
