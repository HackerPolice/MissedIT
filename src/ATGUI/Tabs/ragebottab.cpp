#include "legitbottab.h"
#include "ragebottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../atgui.h"
#include "../../Utils/ColorPickerButton.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static const char *HitChanceType[2] = {
		"Normal",
		"Force Accuracy",
};

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;

//static bool enabled = false;
static bool silent = false;
static bool friendly = false;
static bool closestBone = false;
static bool desireBones[] = {true, true, true, true, true, true};
static bool desiredMultiBones[] = {true, true, true, true, true, true};
static bool autoPistolEnabled = false;
static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static int HitChance = 0.f;
static float MinDamage = 0.f;
static float DamageOverride = 0.f;
static bool autoSlow = false;
static bool doubleFire = false;
static bool scopeControlEnabled = false;
static bool AutoCrouch = false;
static bool OnShot = false;
static bool OnSHotOnKey = false;
static HitchanceType hitchanceType = HitchanceType::Normal;
static int weaponTypes = 0;
static bool AimStepEnabled = false;
static float AimStepMin = 28.0f;
static float AimStepMax = 29.0f;

static char filterWeapons[32];

void UI::ReloadRageWeaponSettings()
{
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Ragebot::weapons.find(currentWeapon) != Settings::Ragebot::weapons.end()) {
		index = currentWeapon;
	}

	silent = Settings::Ragebot::weapons.at(index).silent;
	friendly = Settings::Ragebot::weapons.at(index).friendly;
	closestBone = Settings::Ragebot::weapons.at(index).closestBone;
	autoPistolEnabled = Settings::Ragebot::weapons.at(index).autoPistolEnabled;
	autoShootEnabled = Settings::Ragebot::weapons.at(index).autoShootEnabled;
	autoScopeEnabled = Settings::Ragebot::weapons.at(index).autoScopeEnabled;
	HitChance = Settings::Ragebot::weapons.at(index).HitChance;
	MinDamage = Settings::Ragebot::weapons.at(index).MinDamage;
	autoSlow = Settings::Ragebot::weapons.at(index).autoSlow;
	doubleFire = Settings::Ragebot::weapons.at(index).DoubleFire;
	scopeControlEnabled = Settings::Ragebot::weapons.at(index).scopeControlEnabled;
	DamageOverride = Settings::Ragebot::weapons.at(index).DamageOverride;
	hitchanceType = Settings::Ragebot::weapons.at(index).hitchanceType;
	OnShot = Settings::Ragebot::weapons.at(index).OnShot;
	OnSHotOnKey = Settings::Ragebot::weapons.at(index).OnShotOnKey;
	AimStepEnabled = Settings::Ragebot::weapons.at(index).aimStepEnabled;
	AimStepMin = Settings::Ragebot::weapons.at(index).aimStepMin;
	AimStepMax = Settings::Ragebot::weapons.at(index).aimStepMax;

	for (int BONE = 0; BONE < 6; BONE++) {
		desireBones[BONE] = Settings::Ragebot::weapons.at(index).desireBones[BONE];
		desiredMultiBones[BONE] = Settings::Ragebot::weapons.at(index).desiredMultiBones[BONE];
	}
}

void UI::UpdateRageWeaponSettings()
{
	if (Settings::Ragebot::weapons.find(currentWeapon) == Settings::Ragebot::weapons.end() &&
	    Settings::Ragebot::enabled) {
		Settings::Ragebot::weapons[currentWeapon] = RageWeapon_t();
	}

	RageWeapon_t settings = {
			.silent = silent,
			.friendly = friendly,
			.closestBone = closestBone,
			.autoPistolEnabled = autoPistolEnabled,
			.autoShootEnabled = autoShootEnabled,
			.autoScopeEnabled = autoScopeEnabled,
			.autoSlow = autoSlow,
			.scopeControlEnabled = scopeControlEnabled,
			.DoubleFire = doubleFire,
			.AutoCrouch = AutoCrouch,
			.OnShot = OnShot,
			.OnShotOnKey = OnSHotOnKey,
			.MinDamage = MinDamage,
			.DamageOverride = DamageOverride,
	};

	settings.HitChance = HitChance;
	settings.hitchanceType = hitchanceType;

	for (int BONE = 0; BONE < 6; BONE++) {
		settings.desireBones[BONE] = desireBones[BONE];
		settings.desiredMultiBones[BONE] = desiredMultiBones[BONE];
	}

	Settings::Ragebot::weapons.at(currentWeapon) = settings;

	if (Settings::Ragebot::weapons.at(currentWeapon) == Settings::Ragebot::weapons.at(ItemDefinitionIndex::INVALID) &&
	    currentWeapon != ItemDefinitionIndex::INVALID) {
		Settings::Ragebot::weapons.erase(currentWeapon);
		UI::ReloadRageWeaponSettings();
		return;
	}
}

static void Aimbot()
{
	// all the auto shoot related features are in this section

	if (ImGui::CheckboxFill(XORSTR("Auto Shoot"), &autoShootEnabled)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("AutoSlow"), &autoSlow)) {
		UI::UpdateRageWeaponSettings();
	}

	switch (currentWeapon) {
		case ItemDefinitionIndex::WEAPON_DEAGLE:
		case ItemDefinitionIndex::WEAPON_ELITE:
		case ItemDefinitionIndex::WEAPON_FIVESEVEN:
		case ItemDefinitionIndex::WEAPON_GLOCK:
		case ItemDefinitionIndex::WEAPON_TEC9:
		case ItemDefinitionIndex::WEAPON_HKP2000:
		case ItemDefinitionIndex::WEAPON_USP_SILENCER:
		case ItemDefinitionIndex::WEAPON_P250:
		case ItemDefinitionIndex::WEAPON_CZ75A:
		case ItemDefinitionIndex::WEAPON_REVOLVER:
			break;
		default:
			if (ImGui::CheckboxFill(XORSTR("Auto Scope"), &autoScopeEnabled)) {
				UI::UpdateRageWeaponSettings();
			}
			break;
	}
	switch (currentWeapon) {
		case ItemDefinitionIndex::INVALID:
		case ItemDefinitionIndex::WEAPON_DEAGLE:
		case ItemDefinitionIndex::WEAPON_ELITE:
		case ItemDefinitionIndex::WEAPON_FIVESEVEN:
		case ItemDefinitionIndex::WEAPON_GLOCK:
		case ItemDefinitionIndex::WEAPON_TEC9:
		case ItemDefinitionIndex::WEAPON_HKP2000:
		case ItemDefinitionIndex::WEAPON_USP_SILENCER:
		case ItemDefinitionIndex::WEAPON_P250:
		case ItemDefinitionIndex::WEAPON_CZ75A:
		case ItemDefinitionIndex::WEAPON_REVOLVER:
			if (ImGui::CheckboxFill(XORSTR("Auto Pistol"), &autoPistolEnabled)) {
				UI::UpdateRageWeaponSettings();
			}
			break;
		default:
			break;
	}

	ImGui::PushItemWidth(-1);
	if (HitChance == 0) {
		if (ImGui::SliderInt(XORSTR("##HITCHANCE"), &HitChance, 0, 100, XORSTR("HitChance Off"))) {
			UI::UpdateRageWeaponSettings();
		}
	} else {
		if (ImGui::SliderInt(XORSTR("##HITCHANCE"), &HitChance, 0, 100, XORSTR("Hitchance %d"))) {
			UI::UpdateRageWeaponSettings();
		}
		if (ImGui::Combo(XORSTR("##HitChanceType"), (int *) &hitchanceType, HitChanceType,
		                 IM_ARRAYSIZE(HitChanceType))) {
			UI::UpdateRageWeaponSettings();
		}
	}

	ImGui::PopItemWidth();
}

static void OtherSettings()
{
	// Others Settings for Aimbot basically those that are not directly related to aimbot
	if (ImGui::CheckboxFill(XORSTR("Silent Aim"), &silent)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("Auto Crouch"), &AutoCrouch)) {
		UI::UpdateRageWeaponSettings();
	}

	ImGui::CheckboxFill(XORSTR("LagComp"), &Settings::LagComp::enabled);
	ImGui::Columns();
	{
		if (ImGui::CheckboxFill(XORSTR("LagComp"), &Settings::LagComp::enabled)) {
			hitchanceType = HitchanceType::Normal;
			UI::UpdateRageWeaponSettings(); // because if we need force accuracy our aimbot will not shoot
		}
		ToolTip::Show(
				XORSTR("LagComp is synced with ragebot now but this feature is not fully tested so it can cause crash some times :)"),
				ImGui::IsItemHovered());
	}
	ImGui::Columns();
	{
		if (ImGui::CheckboxFill(XORSTR("BackTrack"), &Settings::BackTrack::enabled)) {
			hitchanceType = HitchanceType::Normal;
			UI::UpdateRageWeaponSettings(); // because if we need force accuracy our aimbot will not shoot
		}
		ToolTip::Show(
				XORSTR("Backtrack is synced with ragebot now but this feature is not fully tested so it can cause crash some times :)"),
				ImGui::IsItemHovered());
	}

	ImGui::CheckboxFill(XORSTR("Resolver"), &Settings::Resolver::resolveAll);
}

static void DamageSettings()
{

	ImGui::PushItemWidth(-1);

	if (MinDamage == 0) {
		if (ImGui::SliderFloat(XORSTR("##VISIBLEDMG"), &MinDamage, 0, 150, XORSTR("Auto Damage"))) {
			UI::UpdateRageWeaponSettings();
		}
	} else {
		if (ImGui::SliderFloat(XORSTR("##VISIBLEDMG"), &MinDamage, 0, 150, XORSTR("Min Damage: %.0f"))) {
			UI::UpdateRageWeaponSettings();
		}
	}

	if (DamageOverride == 0) {
		if (ImGui::SliderFloat(XORSTR("##DamageOverride"), &DamageOverride, 0, 150, XORSTR("DamageOverride OFf"))) {
			UI::UpdateRageWeaponSettings();
		}
	} else {
		if (ImGui::SliderFloat(XORSTR("##DamageOverride"), &DamageOverride, 0, 150, XORSTR("Override Damage: %.0f"))) {
			UI::UpdateRageWeaponSettings();
		}
	}

	UI::KeyBindButton(&Settings::Ragebot::DamageOverrideBtn);

	ImGui::PopItemWidth();

}

static void HitBone()
{

	ImGui::PushItemWidth(-1);

	if (ImGui::Selectable(XORSTR("HEAD"), &desireBones[(int) DesireBones::BONE_HEAD])) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("UPPER CHEST"), &desireBones[(int) DesireBones::UPPER_CHEST])) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("MIDDLE CHEST"), &desireBones[(int) DesireBones::MIDDLE_CHEST])) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("LOWER CHEST"), &desireBones[(int) DesireBones::LOWER_CHEST])) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("HIP"), &desireBones[(int) DesireBones::BONE_HIP])) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("LOWER BODY"), &desireBones[(int) DesireBones::LOWER_BODY])) {
		UI::UpdateRageWeaponSettings();
	}

	ImGui::PopItemWidth();
}

static void MultiBone()
{

	ImGui::PushItemWidth(-1);

	if (ImGui::Selectable(XORSTR("HEAD M"), &desiredMultiBones[(int) DesireBones::BONE_HEAD],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("UPPER CHEST M"), &desiredMultiBones[(int) DesireBones::UPPER_CHEST],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("MIDDLE CHEST M"), &desiredMultiBones[(int) DesireBones::MIDDLE_CHEST],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("LOWER CHEST M"), &desiredMultiBones[(int) DesireBones::LOWER_CHEST],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("HIP M"), &desiredMultiBones[(int) DesireBones::BONE_HIP],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::Selectable(XORSTR("LOWER BODY M"), &desiredMultiBones[(int) DesireBones::LOWER_BODY],
	                      ImGuiSelectableFlags_DontClosePopups)) {
		UI::UpdateRageWeaponSettings();
	}

	ImGui::PopItemWidth();
}

static void OnshotSettings()
{
	if (ImGui::CheckboxFill(XORSTR("On Shot"), &OnShot)) {
		UI::UpdateRageWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("On Shot On Key"), &OnSHotOnKey)) {
		UI::UpdateRageWeaponSettings();
	}
	UI::KeyBindButton(&Settings::Ragebot::OnShotBtn);
}

void Ragebot::Guns()
{
	// straight pested from nimbus
	const char *tabs[] =
			{
					"All",
					"Pistol",
					"Shotgun",
					"SMG",
					"Rifle",
					"Sniper"
			};

	for (int i = 0; i < IM_ARRAYSIZE(tabs); i++) {
		int distance = i == weaponTypes ? 0 : i > weaponTypes ? i - weaponTypes : weaponTypes - i;

		ImGui::GetStyle().Colors[ImGuiCol_Button] = ImVec4(
				Settings::UI::mainColor.Color().Value.x - (distance * 0.035f),
				Settings::UI::mainColor.Color().Value.y - (distance * 0.035f),
				Settings::UI::mainColor.Color().Value.z - (distance * 0.035f),
				Settings::UI::mainColor.Color().Value.w);

		if (ImGui::Button(tabs[i], ImVec2(ImGui::GetWindowSize().x / IM_ARRAYSIZE(tabs) - 9, 0))) {
			if (i == 0) {
				weaponTypes = -1;
			}
		}

		ImGui::GetStyle().Colors[ImGuiCol_Button] = Settings::UI::accentColor.Color();

		if (i < IM_ARRAYSIZE(tabs) - 1) {
			ImGui::SameLine();
		}
	}
}

static void AimStep()
{
	if (ImGui::CheckboxFill(XORSTR("##AimStep"), &AimStepEnabled)) {
		UI::UpdateRageWeaponSettings();
	}

	if (AimStepEnabled) {
		if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &AimStepMin, 28.0f, 180.0f, XORSTR("MIN : %0.0f"))) {
			UI::UpdateRageWeaponSettings();
		}

		if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &AimStepMax, (AimStepMin) + 1.0f, 180.0f, XORSTR("MAX : %0.0f"))) {
			UI::UpdateRageWeaponSettings();
		}
	}
}

void Ragebot::RenderTab()
{
	if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Ragebot::enabled)) {
		Settings::Legitbot::enabled = false;
		UI::UpdateRageWeaponSettings();
	}

	// Guns();

	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, 200);
		ImGui::PushItemWidth(-10);
		{
			ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
		}
		ImGui::PopItemWidth();
		ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, -1));
		for (auto it : ItemDefinitionIndexMap) {
			bool isDefault = (int) it.first < 0;
			if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)),
			                                  Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str()))) {
				continue;
			}

			if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) || Util::Items::IsUtility(it.first)) {
				continue;
			}

			const bool item_selected = ((int) it.first == (int) currentWeapon);
			ImGui::PushID((int) it.first);
			std::string formattedName;
			char changeIndicator = ' ';
			bool isChanged = Settings::Ragebot::weapons.find(it.first) != Settings::Ragebot::weapons.end();
			if (!isDefault && isChanged) {
				changeIndicator = '*';
			}
			formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str()
			                                             : Util::Items::GetItemDisplayName(it.first));
			if (ImGui::Selectable(formattedName.c_str(), item_selected)) {
				currentWeapon = it.first;
				UI::ReloadRageWeaponSettings();
			}
			ImGui::PopID();
		}
		ImGui::ListBoxFooter();
	}
	ImGui::NextColumn();
	{
		ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);

		ImGui::BeginChild(XORSTR("col1"), ImVec2(-1, -1), false);
		{
			// auto Shoot
			ImGui::BeginGroupPanel(XORSTR("AimBot"));
			{
				Aimbot();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("OnShot"));
			{
				OnshotSettings();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Others"));
			{
				OtherSettings();
			}
			ImGui::EndGroupPanel();
			// END Auto Shoot Features
		}
		ImGui::EndChild();
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
		{
			ImGui::BeginGroupPanel(XORSTR("HitBox"));
			{
				HitBone();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("MultiBone"));
			{
				MultiBone();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("AimStep"));
			{
				AimStep();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Damage"));
			{
				DamageSettings();
			}
			ImGui::EndGroupPanel();

			ImGui::Spacing();
			ImGui::Spacing();
			if (currentWeapon > ItemDefinitionIndex::INVALID &&
			    Settings::Ragebot::weapons.find(currentWeapon) != Settings::Ragebot::weapons.end()) {
				if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0))) {
					Settings::Ragebot::weapons.erase(currentWeapon);
					UI::ReloadRageWeaponSettings();
				}
			}
		}
		ImGui::EndChild();
	}
	ImGui::EndColumns();
}

void Ragebot::RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{

	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 150),
	                    10);
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Ragebot::enabled)) {
			Settings::Legitbot::enabled = false;
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::EndGroup();
	ToolTip::Show(XORSTR("Enable Legit Bot"), ImGui::IsItemHovered());

	if (!Settings::Ragebot::enabled) {
		goto DoNotRender;
	}

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 350);
			ImGui::PushItemWidth(-10);
			{
				ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
			}
			ImGui::PopItemWidth();
			ImGui::ListBoxHeader(XORSTR("##GUNS"), ImVec2(-1, 490));
			for (auto it : ItemDefinitionIndexMap) {
				bool isDefault = (int) it.first < 0;
				if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)),
				                                  Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str()))) {
					continue;
				}

				if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) ||
				    Util::Items::IsUtility(it.first)) {
					continue;
				}

				const bool item_selected = ((int) it.first == (int) currentWeapon);
				ImGui::PushID((int) it.first);
				std::string formattedName;
				char changeIndicator = ' ';
				bool isChanged = Settings::Ragebot::weapons.find(it.first) != Settings::Ragebot::weapons.end();
				if (!isDefault && isChanged) {
					changeIndicator = '*';
				}
				formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str()
				                                             : Util::Items::GetItemDisplayName(it.first));
				if (ImGui::Selectable(formattedName.c_str(), item_selected)) {
					currentWeapon = it.first;
					UI::ReloadRageWeaponSettings();
				}
				ImGui::PopID();
			}
			ImGui::ListBoxFooter();

		}
		ImGui::NextColumn();
		{
			// ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);
			// ImGui::beg
			ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
			{
				ImGui::BeginGroupPanel(XORSTR("AimBot"));
				{
					Aimbot();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("OnShot"));
				{
					OnshotSettings();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Others"));
				{
					OtherSettings();
				}
				ImGui::EndGroupPanel();

			}
			ImGui::EndChild();

		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
			{
				ImGui::BeginGroupPanel(XORSTR("HitBox"));
				{
					HitBone();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("MultiBone"));
				{
					MultiBone();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("AimStep"));
				{
					AimStep();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Damage"));
				{
					DamageSettings();
				}
				ImGui::EndGroupPanel();

				ImGui::Spacing();
				if (currentWeapon > ItemDefinitionIndex::INVALID &&
				    Settings::Ragebot::weapons.find(currentWeapon) != Settings::Ragebot::weapons.end()) {
					if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0))) {
						Settings::Ragebot::weapons.erase(currentWeapon);
						UI::ReloadRageWeaponSettings();
					}
				}
			}
			ImGui::EndChild();

		}
		ImGui::EndColumns();

	}
	ImGui::EndGroup();

	DoNotRender:;
}

