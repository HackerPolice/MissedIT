#include "misctab.h"

#include <sys/stat.h>
#include <sstream>

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

#pragma GCC diagnostic ignored "-Wformat-security"

static char nickname[127] = "";
static const char* strafeTypes[] = { "Forwards", "Backwards", "Left", "Right", "Rage" };
static const char* animationTypes[] = { "Static", "Marquee", "Words", "Letters" };
static const char* spammerTypes[] = { "Type", "Normal", "Positions" };
static const char* teams[] = { "Allies", "Enemies", "Both" };
static const char* grenadeTypes[] = { "FLASH", "SMOKE", "MOLOTOV", "HEGRENADE" };
static const char* throwTypes[] = { "NORMAL", "RUN", "JUMP", "WALK" };

static void BunnyHopSection(){

	ImGui::CheckboxFill(XORSTR("##BunnyHop"), &Settings::BHop::enabled);
	ImGui::SameLine();
	ImGui::Text("Enable");

	ImGui::CheckboxFill(XORSTR("Hop Chance"), &Settings::BHop::Chance::enabled);
	if (Settings::BHop::Chance::enabled){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt(XORSTR("##BHOPCHANCE"), &Settings::BHop::Chance::value, 0, 100);
		ImGui::PopItemWidth();
	}

	ImGui::CheckboxFill(XORSTR("Min Hops"), &Settings::BHop::Hops::enabledMin);
	if (Settings::BHop::Hops::enabledMin){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt(XORSTR("##BHOPMIN"), &Settings::BHop::Hops::Min, 0, 20);
		ImGui::PopItemWidth();
	}

	ImGui::CheckboxFill(XORSTR("Max Hops"), &Settings::BHop::Hops::enabledMax);
	if (Settings::BHop::Hops::enabledMax){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderInt(XORSTR("##BHOPMAX"), &Settings::BHop::Hops::Max, 0, 20);
		ImGui::PopItemWidth();
	}

	ImGui::CheckboxFill(XORSTR("Auto Strafe"), &Settings::AutoStrafe::enabled);
	if (Settings::AutoStrafe::enabled){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::PushItemWidth(Settings::AutoStrafe::type == AutostrafeType::AS_RAGE ? ImGui::CalcItemWidth() : -1);
		ImGui::Combo(XORSTR("##STRAFETYPE"), (int*)& Settings::AutoStrafe::type, strafeTypes, IM_ARRAYSIZE(strafeTypes));
		if (Settings::AutoStrafe::type == AutostrafeType::AS_RAGE)
		{
			Settings::AutoStrafe::silent = true;
		}
		else if ( Settings::AutoStrafe::silent ){
			Settings::AutoStrafe::silent = false;
		}
		ImGui::PopItemWidth();
	}

	ImGui::CheckboxFill(XORSTR("Edge Jump"), &Settings::EdgeJump::enabled);
	if ( Settings::EdgeJump::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		UI::KeyBindButton(&Settings::EdgeJump::key);
		ImGui::PopItemWidth();
	}
}

static void KillSpamSection(){
	
	ImGui::CheckboxFill(XORSTR("Kill Messages"), &Settings::Spammer::KillSpammer::enabled);
	if ( Settings::Spammer::KillSpammer::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button(XORSTR("Massages###KILL"), ImVec2(-1,0)))
			ImGui::OpenPopup(XORSTR("options_kill"));
		ImGui::PopItemWidth();
		ImGui::CheckboxFill(XORSTR("Team Chat Only###SAY_TEAM1"), &Settings::Spammer::KillSpammer::sayTeam);
	}
	ImGui::CheckboxFill(XORSTR("Radio Commands On Kill"), &Settings::Spammer::RadioSpammer::enabled);
	
	ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
	if (ImGui::BeginPopup(XORSTR("options_kill")))
	{
		static int killSpammerMessageCurrent = -1;
		static char killSpammerMessageBuf[126];

		ImGui::PushItemWidth(445);
		ImGui::InputText(XORSTR("###SPAMMERMESSAGE"), killSpammerMessageBuf, IM_ARRAYSIZE(killSpammerMessageBuf));
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button(XORSTR("Add")))
		{
			if (strlen(killSpammerMessageBuf) > 0)
				Settings::Spammer::KillSpammer::messages.push_back(std::string(killSpammerMessageBuf));

			strcpy(killSpammerMessageBuf, "");
		}
		ImGui::SameLine();

		if (ImGui::Button(XORSTR("Remove")))
			if (killSpammerMessageCurrent > -1 && (int) Settings::Spammer::KillSpammer::messages.size() > killSpammerMessageCurrent)
				Settings::Spammer::KillSpammer::messages.erase(Settings::Spammer::KillSpammer::messages.begin() + killSpammerMessageCurrent);

		ImGui::PushItemWidth(550);
		ImGui::ListBox("", &killSpammerMessageCurrent, Settings::Spammer::KillSpammer::messages, 10);
		ImGui::PopItemWidth();

		ImGui::EndPopup();
	}

}

static void ChatSpamSection(){

	ImGui::PushItemWidth(-1);
	ImGui::Combo(XORSTR("###SPAMMERTYPE"), (int*)&Settings::Spammer::type, spammerTypes, IM_ARRAYSIZE(spammerTypes));
	if (Settings::Spammer::type != SpammerType::SPAMMER_NONE ){
		ImGui::CheckboxFill(XORSTR("Team Chat###SAY_TEAM2"), &Settings::Spammer::say_team);
		ImGui::PushItemWidth(-1);

		if ( Settings::Spammer::type != SpammerType::SPAMMER_NORMAL )
			if ( ImGui::Button(XORSTR("Massages###SPAMMER"), ImVec2(-1, 0)) )
				ImGui::OpenPopup(XORSTR("options_spammer"));
		if ( Settings::Spammer::type != SpammerType::SPAMMER_POSITIONS )
			if ( ImGui::Button(XORSTR("Options###SPAMMER"), ImVec2(-1, 0)) )
				ImGui::OpenPopup(XORSTR("options_spammer"));

		ImGui::PopItemWidth();
	}
	ImGui::PopItemWidth();

	if (Settings::Spammer::type == SpammerType::SPAMMER_NORMAL)
		ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
	else if (Settings::Spammer::type == SpammerType::SPAMMER_POSITIONS)
		ImGui::SetNextWindowSize(ImVec2(200, 240), ImGuiSetCond_Always);

	if (Settings::Spammer::type != SpammerType::SPAMMER_NONE && ImGui::BeginPopup(XORSTR("options_spammer")))
	{
		if (Settings::Spammer::type == SpammerType::SPAMMER_NORMAL)
		{
			static int spammerMessageCurrent = -1;
			static char spammerMessageBuf[126];

			ImGui::PushItemWidth(445);
			ImGui::InputText(XORSTR("###SPAMMERMESSAGE"), spammerMessageBuf, IM_ARRAYSIZE(spammerMessageBuf));
			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button(XORSTR("Add")))
			{
				if (strlen(spammerMessageBuf) > 0)
					Settings::Spammer::NormalSpammer::messages.push_back(std::string(spammerMessageBuf));

				strcpy(spammerMessageBuf, "");
			}
			ImGui::SameLine();

			if (ImGui::Button(XORSTR("Remove")))
				if (spammerMessageCurrent > -1 && (int) Settings::Spammer::NormalSpammer::messages.size() > spammerMessageCurrent)
					Settings::Spammer::NormalSpammer::messages.erase(Settings::Spammer::NormalSpammer::messages.begin() + spammerMessageCurrent);

			ImGui::PushItemWidth(550);
			ImGui::ListBox("", &spammerMessageCurrent, Settings::Spammer::NormalSpammer::messages, 10);
			ImGui::PopItemWidth();
		}
		else if (Settings::Spammer::type == SpammerType::SPAMMER_POSITIONS)
		{
			ImGui::PushItemWidth(185);
			ImGui::Combo(XORSTR("###POSITIONSTEAM"), &Settings::Spammer::PositionSpammer::team, teams, IM_ARRAYSIZE(teams));
			ImGui::PopItemWidth();
			ImGui::Separator();
			ImGui::CheckboxFill(XORSTR("Show Name"), &Settings::Spammer::PositionSpammer::showName);
			ImGui::CheckboxFill(XORSTR("Show Weapon"), &Settings::Spammer::PositionSpammer::showWeapon);
			ImGui::CheckboxFill(XORSTR("Show Rank"), &Settings::Spammer::PositionSpammer::showRank);
			ImGui::CheckboxFill(XORSTR("Show Wins"), &Settings::Spammer::PositionSpammer::showWins);
			ImGui::CheckboxFill(XORSTR("Show Health"), &Settings::Spammer::PositionSpammer::showHealth);
			ImGui::CheckboxFill(XORSTR("Show Money"), &Settings::Spammer::PositionSpammer::showMoney);
			ImGui::CheckboxFill(XORSTR("Show Last Place"), &Settings::Spammer::PositionSpammer::showLastplace);
		}

		ImGui::EndPopup();
	}
}

static void FovSection(){

	
	ImGui::CheckboxFill(XORSTR("FOV"), &Settings::FOVChanger::enabled);
	if ( Settings::FOVChanger::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat(XORSTR("##FOV"), &Settings::FOVChanger::value, 0, 180, XORSTR("%0.f°"));
		ImGui::PopItemWidth();
	}
	ImGui::CheckboxFill(XORSTR("Viewmodel FOV"), &Settings::FOVChanger::viewmodelEnabled);
	if ( Settings::FOVChanger::viewmodelEnabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::SliderFloat(XORSTR("##MODELFOVAMOUNT"), &Settings::FOVChanger::viewmodelValue, 0, 360, XORSTR("%0.f°"));
		ImGui::PopItemWidth();
	}

	if ( Settings::FOVChanger::viewmodelEnabled || Settings::FOVChanger::enabled  )
		ImGui::CheckboxFill(XORSTR("Ignore Scope"), &Settings::FOVChanger::ignoreScope);
}

static void ThirdPersonSection(){

	ImGui::CheckboxFill(XORSTR("##EnableThirdPerson"), &Settings::ThirdPerson::enabled);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Enable"));

	ImGui::PushItemWidth(-1);
	ImGui::SliderFloat(XORSTR("##CameraOffset"), &Settings::ThirdPerson::distance, 0.f, 500.f, XORSTR("Camera Offset: %0.f"));
	UI::KeyBindButton(&Settings::ThirdPerson::toggleThirdPerson);
	ImGui::PopItemWidth();

}

static void GranadeHelperSection(){
	
	ImGui::CheckboxFill(XORSTR("###ENABLEGRANADEHELPER"), &Settings::GrenadeHelper::enabled);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Enable"));
	
	ImGui::CheckboxFill(XORSTR("##OnlyMatching"), &Settings::GrenadeHelper::onlyMatchingInfos);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Only Matchmaking"));

	
	if (ImGui::Button(XORSTR("Aimassist"), ImVec2(-1, 0)))
		ImGui::OpenPopup(XORSTR("optionAimAssist"));
	ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiSetCond_Always);
	
	if (ImGui::BeginPopup(XORSTR("optionAimAssist")))
	{
		ImGui::PushItemWidth(-1);
		ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::GrenadeHelper::aimAssist);
		ImGui::SliderFloat(XORSTR("###aimstep"), &Settings::GrenadeHelper::aimStep, 0, 10, "Speed: %0.3f");
		ImGui::SliderFloat(XORSTR("###aimfov"), &Settings::GrenadeHelper::aimFov, 0, 180, "Fov: %0.2f");
		ImGui::SliderFloat(XORSTR("###aimdistance"), &Settings::GrenadeHelper::aimDistance, 0, 100, "Distance: %0.2f");
		ImGui::PopItemWidth();
		ImGui::EndPopup();
	}
	
	
	if (ImGui::Button(XORSTR("Add Info"), ImVec2(-1, 0)))
		ImGui::OpenPopup(XORSTR("addinfo_throw"));
	ImGui::SetNextWindowSize(ImVec2(565, 268), ImGuiSetCond_Always);
	
	if (ImGui::BeginPopup(XORSTR("addinfo_throw")))
	{
		static int throwMessageCurrent = -1;
		static char inputName[40];
		static int tType = (int)ThrowType::NORMAL;
		static int gType = (int)GrenadeType::SMOKE;

		ImGui::Columns(1);
		ImGui::PushItemWidth(500);
		ImGui::InputText("", inputName, sizeof(inputName));
		ImGui::PopItemWidth();
		ImGui::SameLine();
		if (ImGui::Button(XORSTR("Add")) && engine->IsInGame() && Settings::GrenadeHelper::actMapName.length() > 0)
		{
			C_BasePlayer* localPlayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());
			if (strlen(inputName) > 0)
			{
				GrenadeInfo n = GrenadeInfo((GrenadeType)gType, localPlayer->GetEyePosition(), *localPlayer->GetVAngles(), (ThrowType)tType, inputName);
				Settings::GrenadeHelper::grenadeInfos.push_back(n);
				std::ostringstream path;
				path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
				if (!DoesFileExist(path.str().c_str()))
					mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				path << XORSTR("/config.json");
				Settings::SaveGrenadeInfo(path.str());
			}
			
			strcpy(inputName, "");
		}
		
			
		ImGui::Combo(XORSTR("###Throwtype"), &tType, throwTypes, IM_ARRAYSIZE(throwTypes));
			
		ImGui::Combo(XORSTR("###Grenadetype"), &gType, grenadeTypes, IM_ARRAYSIZE(grenadeTypes));
		
		ImGui::PushItemWidth(550);
		auto lambda =[](void* data, int idx, const char** out_text)
		{
			*out_text = Settings::GrenadeHelper::grenadeInfos.at(idx).name.c_str();
			return *out_text != nullptr;
		};
		
		ImGui::ListBox("", &throwMessageCurrent, lambda, nullptr, Settings::GrenadeHelper::grenadeInfos.size(), 7);
		ImGui::PopItemWidth();
			
		if (ImGui::Button(XORSTR("Remove"),  ImVec2(ImGui::GetWindowWidth(), 30)))
			if (throwMessageCurrent > -1 && (int) Settings::GrenadeHelper::grenadeInfos.size() > throwMessageCurrent)
			{
				Settings::GrenadeHelper::grenadeInfos.erase(Settings::GrenadeHelper::grenadeInfos.begin() + throwMessageCurrent);
				std::ostringstream path;
				path << GetGhConfigDirectory() << Settings::GrenadeHelper::actMapName;
				if (!DoesFileExist(path.str().c_str()))
					mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				path << XORSTR("/config.json");
				Settings::SaveGrenadeInfo(path.str());
			}
		ImGui::EndPopup();
	}
}

static void ClanTagSection(){
	
	ImGui::CheckboxFill(XORSTR("##EnabledClanTagChanger"), &Settings::ClanTagChanger::enabled);
	ImGui::SameLine();
	ImGui::Text(XORSTR("Enable"));
			
	if ( Settings::ClanTagChanger::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		ImGui::InputText(XORSTR("##CLANTAG"), Settings::ClanTagChanger::value, 30);
		ImGui::PopItemWidth();
	}
	
	ImGui::PushItemWidth(-1);
	if (ImGui::Combo(XORSTR("##ANIMATIONTYPE"), (int*)&Settings::ClanTagChanger::type, animationTypes, IM_ARRAYSIZE(animationTypes)))
		ClanTagChanger::UpdateClanTagCallback();
	ImGui::PopItemWidth();

	if ( Settings::ClanTagChanger::type != ClanTagType::STATIC){
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderInt(XORSTR("##ANIMATIONSPEED"), &Settings::ClanTagChanger::animationSpeed, 500, 2000))
			ClanTagChanger::UpdateClanTagCallback();
		ImGui::PopItemWidth();
	}	
				
	ImGui::PushItemWidth(-1);
	if (ImGui::Button(XORSTR("Update Clantag"), ImVec2(-1, 0)))
		ClanTagChanger::UpdateClanTagCallback();
	ImGui::PopItemWidth();
}

static void NameChangerSection(){

	ImGui::InputText(XORSTR("##NICKNAME"), nickname, 127);
	ImGui::SameLine();
	if (ImGui::Button(XORSTR("Set Nickname"), ImVec2(-1, 0)))
		NameChanger::SetName(std::string(nickname).c_str());

	if (ImGui::Button(XORSTR("Glitch Name")))
		NameChanger::SetName("\n\xAD\xAD\xAD");
	ImGui::SameLine();
	if (ImGui::Button(XORSTR("No Name")))
	{
		NameChanger::changes = 0;
		NameChanger::type = NameChanger::NC_Type::NC_NORMAL;
	}

	ImGui::SameLine();
	if (ImGui::Button(XORSTR("Rainbow Name")))
		NameChanger::InitColorChange(NameChanger::NC_Type::NC_RAINBOW);

	ImGui::SameLine();
	if (ImGui::Button(XORSTR("Colorize Name"), ImVec2(-1, 0)))
		ImGui::OpenPopup(XORSTR("optionColorizeName"));
	ImGui::SetNextWindowSize(ImVec2(150, 300), ImGuiSetCond_Always);
	if (ImGui::BeginPopup(XORSTR("optionColorizeName")))
	{
		ImGui::PushItemWidth(-1);
		for (auto& it : NameChanger::colors)
		{
			if (ImGui::Button(it.second, ImVec2(-1, 0)))
				NameChanger::InitColorChange(NameChanger::NC_Type::NC_SOLID, it.first);
		}
		ImGui::PopItemWidth();

		ImGui::EndPopup();
	}
}

static void NameStealerSection(){	
	if (ImGui::CheckboxFill(XORSTR("Name Stealer"), &Settings::NameStealer::enabled))
		NameStealer::entityId = -1;
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	ImGui::Combo("", &Settings::NameStealer::team, teams, IM_ARRAYSIZE(teams));
	ImGui::PopItemWidth();
}

static void OthersSection(){

	
	ImGui::CheckboxFill(XORSTR("Auto Accept"), &Settings::AutoAccept::enabled);
	ImGui::CheckboxFill(XORSTR("Autoblock"), &Settings::Autoblock::enabled);
	if ( Settings::Autoblock::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		UI::KeyBindButton(&Settings::Autoblock::key);
		ImGui::PopItemWidth();
	}
	
	ImGui::CheckboxFill(XORSTR("Jump Throw"), &Settings::JumpThrow::enabled);
	if ( Settings::JumpThrow::enabled ){
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		UI::KeyBindButton(&Settings::JumpThrow::key);
		ImGui::PopItemWidth();
	}

	ImGui::CheckboxFill(XORSTR("Auto Defuse"), &Settings::AutoDefuse::enabled);
	ImGui::CheckboxFill(XORSTR("Sniper Crosshair"), &Settings::SniperCrosshair::enabled);
	ImGui::CheckboxFill(XORSTR("Disable post-processing"), &Settings::DisablePostProcessing::enabled);
	ImGui::CheckboxFill(XORSTR("No Duck Cooldown"), &Settings::NoDuckCooldown::enabled);
	ImGui::CheckboxFill(XORSTR("Show Ranks"), &Settings::ShowRanks::enabled);
	ImGui::CheckboxFill(XORSTR("Attempt NoFall"), &Settings::NoFall::enabled);
	ImGui::CheckboxFill(XORSTR("Ragdoll Gravity"), &Settings::RagdollGravity::enabled);
	ImGui::CheckboxFill(XORSTR("Show Spectator list"), &Settings::ShowSpectators::enabled);
	ImGui::CheckboxFill(XORSTR("Show Player list"), &PlayerList::showWindow);			
	
	
	
	
			

	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(210, 85));
	if (ImGui::BeginPopupModal(XORSTR("Error###UNTRUSTED_FEATURE")))
	{
		ImGui::Text(XORSTR("You cannot use this feature on a VALVE server."));
		ImGui::CheckboxFill(XORSTR("This is not a VALVE server"), &ValveDSCheck::forceUT);

		if (ImGui::Button(XORSTR("OK")))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}

void Misc::RenderTab()
{
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("Child1"), ImVec2(0, 0), false);
		{			
			ImGui::BeginGroupPanel(XORSTR("Bunny Hop"));
            {
                BunnyHopSection();
            }ImGui::EndGroupPanel();

			
			ImGui::BeginGroupPanel(XORSTR("Kill Spammer"));
            {
            	KillSpamSection();
            }ImGui::EndGroupPanel();
		
			ImGui::BeginGroupPanel(XORSTR("Chat Spammer"));
            {
               	ChatSpamSection();
            }ImGui::EndGroupPanel();
			
			
			ImGui::BeginGroupPanel(XORSTR("Feild of view"));
            {
                FovSection();
            }ImGui::EndGroupPanel();
			
			ImGui::BeginGroupPanel(XORSTR("Third Person"));
            {
                ThirdPersonSection();
            }ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Granage Helper"));
            {
                GranadeHelperSection();
            }ImGui::EndGroupPanel();


			ImGui::EndChild();
		}
			
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("Child2"), ImVec2(0, 0), false);
		{
			
			ImGui::BeginGroupPanel(XORSTR("Clan Tag Changer"));
            {
                ClanTagSection();
            }ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Name Changer"));
            {
                NameChangerSection();
            }ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Name Stealer"));
            {
                NameStealerSection();
            }ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Others"));
            {
                OthersSection();
            }ImGui::EndGroupPanel();

			ImGui::EndChild();
		}
	}
}
