#pragma GCC diagnostic ignored "-Wwrite-strings"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "antiaimtab.hpp"

enum
{
	hvh,
	FakeLag,
};

#define GetPercentVal(val, percent) (val * (percent/100.f))

static const char *dsyncTypes[] = {"Type 1", "Type 2"};

void HvH::AntiAim()
{

	ImGui::PushItemWidth(-1);

	ImGui::CheckboxFill(XORSTR("##ENABLEFAKEANGLE"), &Settings::AntiAim::EnableFakAngle);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Enable Fake Angle"));
	ToolTip::Show(XORSTR("This Option Allow You to create a fake player in the server to hide your real bonematrix"),
	              ImGui::IsItemHovered());

	ImGui::SliderFloat(XORSTR("##offset"), &Settings::AntiAim::offset, 0, 360, XORSTR(" Real Offset : %0.0f"));
	ToolTip::Show(XORSTR("Adjust Your Player angle in Y axis Usefull in HVH"), ImGui::IsItemHovered());

	ImGui::Columns();
	{
		if (Settings::AntiAim::offset != 0) { // I am dumb ass forgot to add this
			UI::KeyBindButton(&Settings::AntiAim::InvertKey);
		}
	}
	ImGui::PopItemWidth();
}

void HvH::Jitter()
{
	ImGui::PushItemWidth(-1);
	{
		ImGui::Columns();
		{
			ImGui::CheckboxFill(XORSTR("Sync With Lag"), &Settings::AntiAim::Jitter::SyncWithLag);
			ToolTip::Show(XORSTR("If you turn it off Jitter will work as a anti aim You can invert with Invert Key"),
			              ImGui::IsItemHovered());
		}

		ImGui::Columns();
		{
			if (Settings::AntiAim::Jitter::Value == 0) {
				ImGui::SliderInt(XORSTR("##Jitter"), &Settings::AntiAim::Jitter::Value, 0, 180,
				                 XORSTR(" Jitter Off Increase to Turn On"));
			} else {
				ImGui::SliderInt(XORSTR("##Jitter"), &Settings::AntiAim::Jitter::Value, 0, 180, XORSTR(" Angle : %d"));
				ToolTip::Show(XORSTR("Boop Boop Shaking head enjoy but only fake will shake"), ImGui::IsItemHovered());
			}
		}
	}
	ImGui::PopItemWidth();
}

void HvH::RageFeatures()
{
	ImGui::Columns();
	{
		ImGui::CheckboxFill(XORSTR("##pitchDown"), &Settings::AntiAim::PitchDown);
		ImGui::SameLine();
		ImGui::Text(XORSTR("PitchDown"));
		ToolTip::Show(XORSTR("Make Your Player Look Down"), ImGui::IsItemHovered());
	}
	ImGui::Columns();
	{
		ImGui::CheckboxFill(XORSTR("##FakeJitter"), &Settings::AntiAim::JitterFake);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Fake Jitter"));
		ToolTip::Show(
				XORSTR("It Will Jitter Fake and can break some Cheat resolvers but make sure you turn on jitter too"),
				ImGui::IsItemHovered());
	}
	ImGui::Columns();
	{
		ImGui::CheckboxFill(XORSTR("##AtTheTarget"), &Settings::AntiAim::atTheTarget);
		ImGui::SameLine();
		ImGui::Text(XORSTR("At The Target"));
		ToolTip::Show(XORSTR("Automatically Angle Your player Towords your Enemy"), ImGui::IsItemHovered());
	}
	ImGui::Columns();
	{
		ImGui::CheckboxFill(XORSTR("##AutoDirection"), &Settings::AntiAim::autoDirection);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Auto Direction"));
		ToolTip::Show(XORSTR("AutoMatically Adjust Your Head According to your Enemy"), ImGui::IsItemHovered());
	}

	ImGui::Columns(); // Invert On Shoot
	{
		ImGui::CheckboxFill(XORSTR("##InvertOnShoot"), &Settings::AntiAim::InvertOnShoot);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Invert On Shoot"));
		ToolTip::Show(XORSTR("Will Invert Your AntiAim After every shoot"), ImGui::IsItemHovered());
	}

	ImGui::Columns();
	{
		ImGui::Text(XORSTR("Dsync"));
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::Combo(XORSTR("##DSYNCTYPE"), (int *) &Settings::AntiAim::dsynctype, dsyncTypes,
		             IM_ARRAYSIZE(dsyncTypes));
		ToolTip::Show(XORSTR("Sry Don't Know the names Enjoy :)"), ImGui::IsItemHovered());
		ImGui::PopItemWidth();
	}
}

void HvH::ManualAntiAim()
{
	ImGui::CheckboxFill(XORSTR("##ManualAntiAim"), &Settings::AntiAim::ManualAntiAim::Enable);
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

void HvH::Others()
{

	ImGui::Columns();
	{
		ImGui::CheckboxFill(XORSTR("ShowReal"), &Settings::AntiAim::ShowReal);
		ToolTip::Show(XORSTR("A arrow will show the side of the real"), ImGui::IsItemHovered());
	}
	ImGui::PushItemWidth(-1);

	ImGui::CheckboxFill(XORSTR("FakeDuck"), &Settings::AntiAim::FakeDuck::enabled);
	ImGui::SameLine();
	UI::KeyBindButton(&Settings::AntiAim::FakeDuck::fakeDuckKey);

	ImGui::CheckboxFill(XORSTR("FakeWalk"), &Settings::AntiAim::FakeWalk::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##FakeWalkSPeed"), &Settings::AntiAim::FakeWalk::Speed, 0, 100, XORSTR("Amount: %0.f"));
	if (Settings::AntiAim::FakeWalk::enabled) {
		UI::KeyBindButton(&Settings::AntiAim::FakeWalk::Key);
	}

	ImGui::CheckboxFill(XORSTR("SlowWalk"), &Settings::AntiAim::SlowWalk::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##SlowWalkSPeed"), &Settings::AntiAim::SlowWalk::Speed, 0, 100, XORSTR("Amount: %0.f"));
	if (Settings::AntiAim::SlowWalk::enabled) {
		UI::KeyBindButton(&Settings::AntiAim::SlowWalk::Key);
	}

	ImGui::PopItemWidth();

}

void HvH::FakeLag()
{
	ImGui::PushItemWidth(-1);

	ImGui::CheckboxFill(XORSTR("FakeLag"), &Settings::FakeLag::enabled);
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##FAKELAGAMOUNT"), &Settings::FakeLag::value, 0, 16, XORSTR("Amount: %d"));
	ImGui::CheckboxFill(XORSTR("Impulse FakeLag"), &Settings::FakeLag::impulseLag);
	ToolTip::Show(XORSTR("It Will Lag and then stop lagging for certain Tick Count IDK how to explain this in words"),
	              ImGui::IsItemHovered());

	ImGui::CheckboxFill(XORSTR("OnShot FakeLag"), &Settings::FakeLag::OnShot::Enable);
	ToolTip::Show(XORSTR("Will Lag Upto SpecificTicks On SHot"), ImGui::IsItemHovered());
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##FAKELAGAMOUNTOnshot"), &Settings::FakeLag::OnShot::Value, 0, 16, XORSTR("Amount: %d"));
	ToolTip::Show(XORSTR("Will Lag Upto SpecificTicks On Shot"), ImGui::IsItemHovered());

	ImGui::CheckboxFill(XORSTR("AfterShot FakeLag"), &Settings::FakeLag::AfterShot::Enable);
	ToolTip::Show(XORSTR("Will Lag Upto SpecificTicks After You shot better in picking"), ImGui::IsItemHovered());
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##FAKELAGAMOUNTAfterShot"), &Settings::FakeLag::AfterShot::Value, 0, 16,
	                 XORSTR("Amount: %d"));
	ToolTip::Show(XORSTR("Will Lag Upto SpecificTicks After You shot better in picking"), ImGui::IsItemHovered());

	ImGui::CheckboxFill(XORSTR("InAir FakeLag"), &Settings::FakeLag::InAir::Enable);
	ToolTip::Show(XORSTR("Lag When You Are In Air"), ImGui::IsItemHovered());
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##FAKELAGAMOUNTINAIR"), &Settings::FakeLag::InAir::Value, 0, 16, XORSTR("Amount: %d"));
	ToolTip::Show(XORSTR("Lag When You Are In Air"), ImGui::IsItemHovered());
	ImGui::PopItemWidth();
}

void HvH::LBYBreak()
{
	ImGui::CheckboxFill(XORSTR("Enable"), &Settings::AntiAim::lbyBreak::Enabled);
	ToolTip::Show(XORSTR("Enable Lby Break"), ImGui::IsItemHovered());

	ImGui::CheckboxFill(XORSTR("Hide Break"), &Settings::AntiAim::lbyBreak::notSend);
	ToolTip::Show(XORSTR("Do the send data when you are breaking lby"), ImGui::IsItemHovered());

	ImGui::PushItemWidth(-1);
	{
		ImGui::SliderInt(XORSTR("##LbyAngle"), &Settings::AntiAim::lbyBreak::angle, 0, 180, XORSTR("Amount: %d"));
		ToolTip::Show(XORSTR("Angle to break the LBY you can invert with invert key"), ImGui::IsItemHovered());
	}
	ImGui::PopItemWidth();
}

void HvH::RenderTab()
{
	ImGui::CheckboxFill(XORSTR("Enable"), &Settings::AntiAim::Enabled);

	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel(XORSTR("AntiAim"));
			{
				AntiAim();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Rage"));
			{
				RageFeatures();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Lby Break"));
			{
				LBYBreak();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Jitter"));
			{
				Jitter();
			}
			ImGui::EndGroupPanel();

		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel(XORSTR("Manual AntiAim"));
			{
				ManualAntiAim();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Others"));
			{
				Others();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("FakeLag"));
			{
				FakeLag();
			}
			ImGui::EndGroupPanel();
		}
		ImGui::EndChild();
	}
	ImGui::EndColumns();
}

void HvH::RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{

	if (sideTabIndex == hvh) {

		draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 945, pos.y + 95), ImColor(0, 0, 0, 150),
		                    10);
		ImGui::SetCursorPos(ImVec2(185, 70));
		ImGui::BeginGroup();
		{
			ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::AntiAim::Enabled);
		}
		ImGui::EndGroup();
		ToolTip::Show(XORSTR("Enable AntiAim"), ImGui::IsItemHovered());

		if (!Settings::AntiAim::Enabled) {
			goto DoNotRender;
		}

		ImGui::SetCursorPos(ImVec2(180, 100));
		ImGui::BeginGroup();
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::BeginChild(XORSTR("HVH1"), ImVec2(0, 0), false);
				{

					ImGui::BeginGroupPanel(XORSTR("AntiAim"));
					{
						AntiAim();
					}
					ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Rage"));
					{
						RageFeatures();
					}
					ImGui::EndGroupPanel();

				}
				ImGui::EndChild();

			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild(XORSTR("HVH2"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Lby Break"));
					{
						LBYBreak();
					}
					ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Jitter"));
					{
						Jitter();
					}
					ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Manual AntiAim"));
					{
						ManualAntiAim();
					}
					ImGui::EndGroupPanel();

				}
				ImGui::EndChild();
			}
			ImGui::EndColumns();

		}
		ImGui::EndGroup();
	} else {

		// draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 150), 10);
		// ImGui::SetCursorPos(ImVec2(185, 70));
		// ImGui::BeginGroup();
		// {
		//     ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::FakeLag::enabled);
		// }ImGui::EndGroup();
		// ToolTip::Show(XORSTR("Enable Fakelag"), ImGui::IsItemHovered());

		ImGui::SetCursorPos(ImVec2(180, 65));
		ImGui::BeginGroup();
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::BeginChild(XORSTR("Fakelag1"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("FakeLag"));
					{
						FakeLag();
					}
					ImGui::EndGroupPanel();
				}
				ImGui::EndChild();

			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild(XORSTR("Fakelag2"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Others"));
					{
						Others();
					}
					ImGui::EndGroupPanel();
				}
				ImGui::EndChild();

			}
			ImGui::EndColumns();

		}
		ImGui::EndGroup();

	}

	DoNotRender:;
}
