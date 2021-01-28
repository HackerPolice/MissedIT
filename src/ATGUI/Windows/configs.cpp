#include "configs.h"

void Configs::RenderTab(){
		static std::vector<std::string> configItems = GetConfigs();
		static int configItemCurrent = -1;

		static char buf[128] = "";

		ImGui::PushItemWidth(-1);
		if (buf[0] != '\0' && strlen(buf) > 0)
		{
			if ( ImGui::Button(XORSTR("Add")))
			{
				ToolTip::Show(XORSTR("Clicked"), true);
				std::ostringstream path;
				path << GetConfigDirectory() << buf;

				if (!DoesFileExist(path.str().c_str()))
				{
					mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					path << XORSTR("/config.json");
					Settings::LoadDefaultsOrSave(path.str());

					configItems = GetConfigs();
					configItemCurrent = -1;
				}
			}
			ImGui::SameLine();
		}
		
		if (ImGui::Button(XORSTR("Refresh")))
			configItems = GetConfigs();
		if (configItemCurrent > -1 ){
			ImGui::SameLine();
			if (ImGui::Button(XORSTR("Save")))
			{
				ToolTip::Show(XORSTR("Clicked"), true);
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int) configItems.size()))
				{
					std::ostringstream path;
					path << GetConfigDirectory() << configItems[configItemCurrent] << XORSTR("/config.json");

					Settings::LoadDefaultsOrSave(path.str());
				}
			}

			ImGui::SameLine();
			if (configItemCurrent > -1 && ImGui::Button(XORSTR("Remove")))
			{
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int) configItems.size()))
				{
					std::ostringstream path;
					path << GetConfigDirectory() << configItems[configItemCurrent];

					Settings::DeleteConfig(path.str());

					configItems = GetConfigs();
					configItemCurrent = -1;
				}
			}
		}
		
		
		
		ImGui::InputText("", buf, IM_ARRAYSIZE(buf));

		if (ImGui::ListBox("", &configItemCurrent, configItems ))
		{
			std::ostringstream path;
			path << GetConfigDirectory() << configItems[configItemCurrent] << XORSTR("/config.json");

			Settings::LoadConfig(path.str());
			UI::ReloadRageWeaponSettings();
			UI::ReloadWeaponSettings();
		}
		ImGui::PopItemWidth();
}

static void AimwareConfig(int sideTabClick){
	static std::vector<std::string> configItems = GetConfigs();
		static int configItemCurrent = -1;

		static char buf[128] = "";

		ImGui::PushItemWidth(-1);
		if (buf[0] != '\0' && strlen(buf) > 0)
		{
			if ( ImGui::Button(XORSTR("Add")))
			{
				ToolTip::Show(XORSTR("Clicked"), true);
				std::ostringstream path;
				path << GetConfigDirectory() << buf;

				if (!DoesFileExist(path.str().c_str()))
				{
					mkdir(path.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
					path << XORSTR("/config.json");
					Settings::LoadDefaultsOrSave(path.str());

					configItems = GetConfigs();
					configItemCurrent = -1;
				}
			}
			ImGui::SameLine();
		}
		
		if (ImGui::Button(XORSTR("Refresh")))
			configItems = GetConfigs();
		if (configItemCurrent > -1 ){
			ImGui::SameLine();
			if (ImGui::Button(XORSTR("Save")))
			{
				ToolTip::Show(XORSTR("Clicked"), true);
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int) configItems.size()))
				{
					std::ostringstream path;
					path << GetConfigDirectory() << configItems[configItemCurrent] << XORSTR("/config.json");

					Settings::LoadDefaultsOrSave(path.str());
				}
			}

			ImGui::SameLine();
			if (configItemCurrent > -1 && ImGui::Button(XORSTR("Remove")))
			{
				if (configItems.size() > 0 && (configItemCurrent >= 0 && configItemCurrent < (int) configItems.size()))
				{
					std::ostringstream path;
					path << GetConfigDirectory() << configItems[configItemCurrent];

					Settings::DeleteConfig(path.str());

					configItems = GetConfigs();
					configItemCurrent = -1;
				}
			}
		}
		
		
		
		ImGui::InputText("", buf, IM_ARRAYSIZE(buf));

		if (ImGui::ListBox("", &configItemCurrent, configItems ))
		{
			std::ostringstream path;
			path << GetConfigDirectory() << configItems[configItemCurrent] << XORSTR("/config.json");

			Settings::LoadConfig(path.str());
			UI::ReloadRageWeaponSettings();
			UI::ReloadWeaponSettings();
		}
		ImGui::PopItemWidth();
}

void Configs::RenderAimware(ImVec2 &pos, ImDrawList * draw, int sideTabIndex){
	static bool initial = true;
	ImGui::SetCursorPos(ImVec2(180, 65));
	ImGui::BeginGroup();
	{
		ImGui::Columns();
		{
			AimwareConfig(-1);
		}
		
	}ImGui::EndGroup();
}