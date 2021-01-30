#include "triggerbottab.h"

#pragma GCC diagnostic ignored "-Wformat-security"

void Triggerbot::KeyBind()
{

	ImGui::CheckboxFill(XORSTR("Active On Key"), &Settings::Triggerbot::OnKey::enable);
	ImGui::PushItemWidth(-1);
	UI::KeyBindButton(&Settings::Triggerbot::OnKey::key);
	ImGui::PopItemWidth();
}

void Triggerbot::Delay()
{

	ImGui::CheckboxFill(XORSTR("Randome Delay"), &Settings::Triggerbot::RandomDelay::enabled);
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

	ImGui::CheckboxFill(XORSTR("Auto Knife"), &Settings::AutoKnife::enabled);
	ImGui::CheckboxFill(XORSTR("On Key"), &Settings::AutoKnife::onKey);

	ImGui::CheckboxFill(XORSTR("Enemies"), &Settings::AutoKnife::Filters::enemies);
	ImGui::CheckboxFill(XORSTR("Allies"), &Settings::AutoKnife::Filters::allies);

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
	ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Triggerbot::enabled);
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
	ImGui::EndColumns();
}

void Triggerbot::RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{
	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 150),
	                    10);
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Triggerbot::enabled)) {}
	}
	ImGui::EndGroup();
	ToolTip::Show(XORSTR("Enable Trigger Bot Bot"), ImGui::IsItemHovered());

	if (!Settings::Triggerbot::enabled) {
		goto DoNotRender;
	}
	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
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

			}
			ImGui::EndChild();
		}
		ImGui::EndColumns();

	}
	ImGui::EndGroup();

	DoNotRender:;
}

