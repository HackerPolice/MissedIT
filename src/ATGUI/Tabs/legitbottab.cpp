#include "legitbottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../atgui.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static bool silent = false;
static Bone bone = BONE_HEAD;
static ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
static bool aimkeyOnly = false;
static bool smoothEnabled = false;
static float smoothValue = 0.5f;
static SmoothType smoothType = SmoothType::SLOW_END;
static bool smoothSaltEnabled = false;
static float smoothSaltMultiplier = 0.0f;
static bool errorMarginEnabled = false;
static float errorMarginValue = 0.0f;
static bool autoAimEnabled = false;
static float LegitautoAimValue = 15.0f;
static bool aimStepEnabled = false;
static float aimStepMin = 25.0f;
static float aimStepMax = 35.0f;
static bool rcsEnabled = false;
static bool rcsAlwaysOn = false;
static float rcsAmountX = 2.0f;
static float rcsAmountY = 2.0f;
static bool autoPistolEnabled = false;
static bool autoShootEnabled = false;
static bool autoScopeEnabled = false;
static bool ignoreJumpEnabled = false;
static bool ignoreEnemyJumpEnabled = false;
static bool hitchanceEnabled = false;
static float hitchance = 100.f;
static float mindamagevalue = 10.f;
static bool mindamage = false;
static bool autoSlow = false;
static bool predEnabled = false;
static bool TriggerBot = false;
static bool autowall = false;
static bool PriorityBone = false;

static const char *targets[] =
		{
				"BONE_PELVIS",
				"HIP",
				"LOWER SPINE",
				"MIDDLE SPINE",
				"UPPER SPINE",
				"NECK",
				"HEAD"
		};

void UI::ReloadWeaponSettings()
{
	return;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end()) {
		index = currentWeapon;
	}

	silent = Settings::Legitbot::weapons.at(index).silent;
	autoShootEnabled = Settings::Legitbot::weapons.at(index).autoShoot;
	bone = Settings::Legitbot::weapons.at(index).bone;
	aimkey = Settings::Legitbot::weapons.at(index).aimkey;
	aimkeyOnly = Settings::Legitbot::weapons.at(index).aimkeyOnly;
	smoothEnabled = Settings::Legitbot::weapons.at(index).smoothEnabled;
	smoothValue = Settings::Legitbot::weapons.at(index).smoothAmount;
	smoothType = Settings::Legitbot::weapons.at(index).smoothType;
	smoothSaltEnabled = Settings::Legitbot::weapons.at(index).smoothSaltEnabled;
	smoothSaltMultiplier = Settings::Legitbot::weapons.at(index).smoothSaltMultiplier;
	errorMarginEnabled = Settings::Legitbot::weapons.at(index).errorMarginEnabled;
	errorMarginValue = Settings::Legitbot::weapons.at(index).errorMarginValue;
	autoAimEnabled = Settings::Legitbot::weapons.at(index).autoAimEnabled;
	LegitautoAimValue = Settings::Legitbot::weapons.at(index).LegitautoAimFov;
	aimStepEnabled = Settings::Legitbot::weapons.at(index).aimStepEnabled;
	aimStepMin = Settings::Legitbot::weapons.at(index).aimStepMin;
	aimStepMax = Settings::Legitbot::weapons.at(index).aimStepMax;
	rcsEnabled = Settings::Legitbot::weapons.at(index).rcsEnabled;
	rcsAlwaysOn = Settings::Legitbot::weapons.at(index).rcsAlwaysOn;
	rcsAmountX = Settings::Legitbot::weapons.at(index).rcsAmountX;
	rcsAmountY = Settings::Legitbot::weapons.at(index).rcsAmountY;
	autoPistolEnabled = Settings::Legitbot::weapons.at(index).autoPistolEnabled;
	autoScopeEnabled = Settings::Legitbot::weapons.at(index).autoScopeEnabled;
	ignoreJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreJumpEnabled;
	ignoreEnemyJumpEnabled = Settings::Legitbot::weapons.at(index).ignoreEnemyJumpEnabled;
	hitchanceEnabled = Settings::Legitbot::weapons.at(index).hitchanceEnabled;
	hitchance = Settings::Legitbot::weapons.at(index).hitchance;
	mindamage = Settings::Legitbot::weapons.at(index).mindamage;
	mindamagevalue = Settings::Legitbot::weapons.at(index).minDamagevalue;
	autoSlow = Settings::Legitbot::weapons.at(index).autoSlow;
	predEnabled = Settings::Legitbot::weapons.at(index).predEnabled;
	TriggerBot = Settings::Legitbot::weapons.at(index).TriggerBot;
	autowall = Settings::Legitbot::weapons.at(index).autoWall;
	PriorityBone = Settings::Legitbot::weapons.at(index).PriorityBone;

}

void UI::UpdateWeaponSettings()
{
	return;
	if (Settings::Legitbot::weapons.find(currentWeapon) == Settings::Legitbot::weapons.end() &&
	    Settings::Legitbot::enabled) {
		Settings::Legitbot::weapons[currentWeapon] = LegitWeapon_t();
	}

	LegitWeapon_t settings = {
			.silent = silent,
			.autoShoot = autoShootEnabled,
			.aimkeyOnly = aimkeyOnly,
			.smoothEnabled = smoothEnabled,
			.smoothSaltEnabled = smoothSaltEnabled,
			.errorMarginEnabled = errorMarginEnabled,
			.autoAimEnabled = autoAimEnabled,
			.aimStepEnabled = aimStepEnabled,
			.rcsEnabled = rcsEnabled,
			.rcsAlwaysOn = rcsAlwaysOn,
			.hitchanceEnabled = hitchanceEnabled,
			.autoPistolEnabled = autoPistolEnabled,
			.autoScopeEnabled = autoScopeEnabled,
			.ignoreJumpEnabled = ignoreJumpEnabled,
			.ignoreEnemyJumpEnabled = ignoreEnemyJumpEnabled,
			.autoSlow = autoSlow,
			.predEnabled = predEnabled,
			.TriggerBot = TriggerBot,
			.mindamage = mindamage,
			.autoWall = autowall,
			.PriorityBone = PriorityBone,
			.bone = bone,
			.smoothType = smoothType,
			.aimkey = aimkey,
			.smoothAmount = smoothValue,
			.smoothSaltMultiplier = smoothSaltMultiplier,
			.errorMarginValue = errorMarginValue,
			.LegitautoAimFov = LegitautoAimValue,
			.aimStepMin = aimStepMin,
			.aimStepMax = aimStepMax,
			.rcsAmountX = rcsAmountX,
			.rcsAmountY = rcsAmountY,
			.minDamagevalue = mindamagevalue,
			.hitchance = hitchance,
	};

	Settings::Legitbot::weapons.at(currentWeapon) = settings;

	if (Settings::Legitbot::weapons.at(currentWeapon) == Settings::Legitbot::weapons.at(ItemDefinitionIndex::INVALID) &&
	    currentWeapon != ItemDefinitionIndex::INVALID) {
		Settings::Legitbot::weapons.erase(currentWeapon);
		UI::ReloadWeaponSettings();
		return;
	}
}

static void Aimbot()
{

	if (ImGui::CheckboxFill(XORSTR("##AutoAim"), &autoAimEnabled)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text("Auto Aim");

	if (autoAimEnabled) {
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat(XORSTR("##Fov"), &LegitautoAimValue, 0, 30, XORSTR("FOV : %0.f°"))) {
			UI::UpdateWeaponSettings();
		}
		ImGui::PopItemWidth();
	}

	if (ImGui::CheckboxFill(XORSTR("##AimkeyOnly"), &aimkeyOnly)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text("AimKey Only");

	if (aimkeyOnly) {
		ImGui::SameLine();
		UI::KeyBindButton(&aimkey);
	}

	// ImGui::tab

	if (ImGui::CheckboxFill(XORSTR("##PriorityBone"), &PriorityBone)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text("Priority Bone");
	ToolTip::Show(XORSTR("When you enable it aimbot will always look for the desired bone first"),
	              ImGui::IsItemHovered());
	if (PriorityBone) {
		ImGui::PushItemWidth(-1);
		if (ImGui::Combo(XORSTR("##AIMTARGET"), &bone, targets, IM_ARRAYSIZE(targets))) {
			UI::UpdateWeaponSettings();
		}
		ImGui::PopItemWidth();
	}

}

static void Recoil()
{

	if (ImGui::CheckboxFill(XORSTR("Recoil Control"), &rcsEnabled)) {
		UI::UpdateWeaponSettings();
	}

	if (ImGui::CheckboxFill(XORSTR("ALLWays On"), &rcsAlwaysOn)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::PushItemWidth(-1);
	{
		if (ImGui::SliderFloat(XORSTR("##RCSX"), &rcsAmountX, 0, 2, XORSTR("Pitch: %0.2f"))) {
			UI::UpdateWeaponSettings();
		}
		if (ImGui::SliderFloat(XORSTR("##RCSY"), &rcsAmountY, 0, 2, XORSTR("Yaw: %0.2f"))) {
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::PopItemWidth();
}

static void Semirage()
{

	if (ImGui::CheckboxFill(XORSTR("AutoShoot"), &autoShootEnabled)) {
		UI::UpdateWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("Auto Slow"), &autoSlow)) {
		UI::UpdateWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("AutoWall"), &autowall)) {
		UI::UpdateWeaponSettings();
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
				UI::UpdateWeaponSettings();
			}
			break;
		default:
			break;
	}

	if (ImGui::CheckboxFill(XORSTR("##HitChance"), &hitchanceEnabled)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text(XORSTR("Hitchance"));
	if (hitchanceEnabled) {
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat(XORSTR("##HitchanceValue"), &hitchance, 0, 100, XORSTR("%0.f%%"))) {
			UI::UpdateWeaponSettings();
		}
		ImGui::PopItemWidth();
	}

	if (ImGui::CheckboxFill(XORSTR("##MINDAMAGEENEBLED"), &mindamage)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text(XORSTR("Minimum Damage"));
	if (mindamage) {
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat(XORSTR("##MINDAMAGE"), &mindamagevalue, 0, 100, XORSTR("%.0f"))) {
			UI::UpdateWeaponSettings();
		}
		ImGui::PopItemWidth();
	}
}

static void Others()
{
	if (ImGui::CheckboxFill(XORSTR("Silent Aim"), &silent)) {
		UI::UpdateWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("Prediction"), &predEnabled)) {
		UI::UpdateWeaponSettings();
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
				UI::UpdateWeaponSettings();
			}
	}

	if (ImGui::CheckboxFill(XORSTR("Ignore Jump (Self)"), &ignoreJumpEnabled)) {
		UI::UpdateWeaponSettings();
	}
	if (ImGui::CheckboxFill(XORSTR("Ignore Jump (Enemies)"), &ignoreEnemyJumpEnabled)) {
		UI::UpdateWeaponSettings();
	}
}

static void Humanising()
{
	static const char *smoothTypes[] =
			{
					"Slow Near End",
					"Constant Speed",
					"Fast Near End"
			};

	ImGui::PushItemWidth(-1);
	{
		if (ImGui::CheckboxFill(XORSTR("##Smoothing"), &smoothEnabled)) {
			UI::UpdateWeaponSettings();
		}
		ImGui::SameLine();
		if (ImGui::SliderFloat(XORSTR("##SMOOTH"), &smoothValue, 0, 1, XORSTR("Smoothing : %0.2f"))) {
			UI::UpdateWeaponSettings();
		}
	}

	{
		if (ImGui::CheckboxFill(XORSTR("##SmoothSalting"), &smoothSaltEnabled)) {
			UI::UpdateWeaponSettings();
		}
		ImGui::SameLine();
		if (ImGui::SliderFloat(XORSTR("##SALT"), &smoothSaltMultiplier, 0, smoothValue,
		                       XORSTR("Smooth Salting : %0.2f"))) {
			UI::UpdateWeaponSettings();
		}
	}

	{
		if (ImGui::CheckboxFill(XORSTR("##ErrorMargin"), &errorMarginEnabled)) {
			UI::UpdateWeaponSettings();
		}
		ImGui::SameLine();
		if (ImGui::SliderFloat(XORSTR("##ERROR"), &errorMarginValue, 0, 2, XORSTR("Error In Shot : %0.2f"))) {
			UI::UpdateWeaponSettings();
		}
	}

	{
		if (ImGui::Combo(XORSTR("##SMOOTHTYPE"), (int *) &smoothType, smoothTypes, IM_ARRAYSIZE(smoothTypes))) {
			UI::UpdateWeaponSettings();
		}
	}

	ImGui::PopItemWidth();
}

static void AimStep()
{
	if (ImGui::CheckboxFill(XORSTR("##AimStep"), &aimStepEnabled)) {
		UI::UpdateWeaponSettings();
	}
	ImGui::SameLine();
	ImGui::Text("Enable");

	ImGui::PushItemWidth(-1);
	{
		if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &aimStepMin, 5.0f, 180.0f, XORSTR("MIN : %0.0f"))) {
			UI::UpdateWeaponSettings();
		}

		if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &aimStepMax, (aimStepMin) + 1.0f, 180.0f, XORSTR("MAX : %0.0f"))) {
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::PopItemWidth();
}

void Legitbot::RenderTab()
{
	static char filterWeapons[32];

	if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Legitbot::enabled)) {
		Settings::Ragebot::enabled = false;
		UI::UpdateWeaponSettings();
	}

	ImGui::Columns(3, nullptr, false);
	{
		ImGui::SetColumnOffset(1, 200);
		ImGui::PushItemWidth(-10);

		ImGui::InputText(XORSTR("##FILTERWEAPONS"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
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
			bool isChanged = Settings::Legitbot::weapons.find(it.first) != Settings::Legitbot::weapons.end();
			if (!isDefault && isChanged) {
				changeIndicator = '*';
			}
			formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str()
			                                             : Util::Items::GetItemDisplayName(it.first));
			if (ImGui::Selectable(formattedName.c_str(), item_selected)) {
				currentWeapon = it.first;
				UI::ReloadWeaponSettings();
			}
			ImGui::PopID();
		}
		ImGui::ListBoxFooter();

	}
	ImGui::NextColumn();
	{

		ImGui::SetColumnOffset(2, ImGui::GetWindowWidth() / 2 + 75);
		ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel(XORSTR("AimBot"));
			{
				Aimbot();

			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("RCS"));
			{
				Recoil();

			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Humanising"));
			{
				Humanising();

			}
			ImGui::EndGroupPanel();

		}
		ImGui::EndChild();
	}

	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("COL3"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel(XORSTR("SemiRage"));
			{
				Semirage();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Aim Step"));
			{
				AimStep();
			}
			ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Others"));
			{
				Others();
			}
			ImGui::EndGroupPanel();

			ImGui::Spacing();

			if (currentWeapon > ItemDefinitionIndex::INVALID &&
			    Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end()) {
				if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0))) {
					Settings::Legitbot::weapons.erase(currentWeapon);
					UI::ReloadWeaponSettings();
				}
			}
			ImGui::EndChild();
		}
	}

	ImGui::EndColumns();

}

void Legitbot::RenderAimware(ImVec2 &pos, ImDrawList *draw, int sideTabIndex)
{

	static char filterWeapons[32];

	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 960 - 15, pos.y + 95), ImColor(0, 0, 0, 150),
	                    10);
	ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
	{
		if (ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::Legitbot::enabled)) {
			Settings::Ragebot::enabled = false;
			UI::UpdateWeaponSettings();
		}
	}
	ImGui::EndGroup();
	ToolTip::Show(XORSTR("Enable LegitBot Bot"), ImGui::IsItemHovered());

	if (!Settings::Legitbot::enabled) {
		goto DoNotRender;
	}

	ImGui::SetCursorPos(ImVec2(180, 100));
	ImGui::BeginGroup();
	{
		ImGui::Columns(3, nullptr, false);
		{
			ImGui::SetColumnOffset(1, 350);
			ImGui::PushItemWidth(-10);

			ImGui::InputText(XORSTR("##FILTERWEAPONSAimware"), filterWeapons, IM_ARRAYSIZE(filterWeapons));
			ImGui::PopItemWidth();
			ImGui::ListBoxHeader(XORSTR("##GUNSAimware"), ImVec2(-1, 490));
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
				bool isChanged = Settings::Legitbot::weapons.find(it.first) != Settings::Legitbot::weapons.end();
				if (!isDefault && isChanged) {
					changeIndicator = '*';
				}
				formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str()
				                                             : Util::Items::GetItemDisplayName(it.first));
				if (ImGui::Selectable(formattedName.c_str(), item_selected)) {
					currentWeapon = it.first;
					UI::ReloadWeaponSettings();
				}
				ImGui::PopID();
			}
			ImGui::ListBoxFooter();

		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
			{
				ImGui::BeginGroupPanel(XORSTR("AimBot"));
				{
					Aimbot();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("RCS"));
				{
					Recoil();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Humanising"));
				{
					Humanising();
				}
				ImGui::EndGroupPanel();

			}
			ImGui::EndChild();

		}
		ImGui::NextColumn();
		{
			ImGui::BeginChild(XORSTR("COL3"), ImVec2(0, 0), false);
			{

				ImGui::BeginGroupPanel(XORSTR("SemiRage"));
				{
					Semirage();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Aim Step"));
				{
					AimStep();
				}
				ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Others"));
				{
					Others();
				}
				ImGui::EndGroupPanel();

				ImGui::Spacing();

				if (currentWeapon > ItemDefinitionIndex::INVALID &&
				    Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end()) {
					if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0))) {
						Settings::Legitbot::weapons.erase(currentWeapon);
						UI::ReloadWeaponSettings();
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

