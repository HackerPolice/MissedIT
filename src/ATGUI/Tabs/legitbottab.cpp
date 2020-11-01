#include "legitbottab.h"
#include "ragebottab.h"

#include "../../SDK/definitions.h"
#include "../../settings.h"
#include "../../Utils/xorstring.h"
#include "../../ImGUI/imgui_internal.h"
#include "../atgui.h"
#include "../Windows/configs.h"
#include "../../Hacks/AimBot/legitbot.h"
#include "triggerbottab.h"

#pragma GCC diagnostic ignored "-Wformat-security"

static ItemDefinitionIndex currentWeapon = ItemDefinitionIndex::INVALID;
static bool silent = false;
static bool desiredBones[] = {true, true, true, true, true, true, true, // center mass
							  true, true, true, true, true, true, true, // left arm
							  true, true, true, true, true, true, true, // right arm
							  true, true, true, true, true, // left leg
							  true, true, true, true, true  // right leg
							 };

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
static bool hitchanceEnaled = false;
static float hitchance = 100.f;
static float mindamagevalue = 10.f;
static bool mindamage = false;
static bool autoSlow = false;
static bool predEnabled = false;
static bool TriggerBot = false;
static bool autowall = false;

void UI::ReloadWeaponSettings()
{
	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
	{
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
	hitchanceEnaled = Settings::Legitbot::weapons.at(index).hitchanceEnaled;
	hitchance = Settings::Legitbot::weapons.at(index).hitchance;
	mindamage = Settings::Legitbot::weapons.at(index).mindamage;
	mindamagevalue = Settings::Legitbot::weapons.at(index).minDamagevalue;
	autoSlow = Settings::Legitbot::weapons.at(index).autoSlow;
	predEnabled = Settings::Legitbot::weapons.at(index).predEnabled;
	TriggerBot = Settings::Legitbot::weapons.at(index).TriggerBot;
	autowall = Settings::Legitbot::weapons.at(index).autoWall;

	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		desiredBones[bone] = Settings::Legitbot::weapons.at(index).desiredBones[bone];
}

void UI::UpdateWeaponSettings()
{
	if (Settings::Legitbot::weapons.find(currentWeapon) == Settings::Legitbot::weapons.end() && Settings::Legitbot::enabled)
	{
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
			.hitchanceEnaled = hitchanceEnaled,
			.autoPistolEnabled = autoPistolEnabled,
			.autoScopeEnabled = autoScopeEnabled,
			.ignoreJumpEnabled = ignoreJumpEnabled,
			.ignoreEnemyJumpEnabled = ignoreEnemyJumpEnabled,
			.autoSlow = autoSlow,
			.predEnabled = predEnabled,
			.TriggerBot = TriggerBot,
			.mindamage = mindamage,
			.autoWall = autowall,
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



	for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++)
		settings.desiredBones[bone] = desiredBones[bone];

	Settings::Legitbot::weapons.at(currentWeapon) = settings;

	if (Settings::Legitbot::weapons.at(currentWeapon) == Settings::Legitbot::weapons.at(ItemDefinitionIndex::INVALID) &&
		currentWeapon != ItemDefinitionIndex::INVALID)
	{
		Settings::Legitbot::weapons.erase(currentWeapon);
		UI::ReloadWeaponSettings();
		return;
	}
}

void Legitbot::Aimbot(){

	static const char* targets[] = 
		{ 
			"PELVIS", 
			"HIP", 
			"LOWER SPINE", 
			"MIDDLE SPINE", 
			"UPPER SPINE", 
			"NECK", 
			"HEAD" 
		};

	if (ImGui::Checkbox(XORSTR("Aimkey Only"), &aimkeyOnly))
		UI::UpdateWeaponSettings();
			
	if (aimkeyOnly)
	{
		ImGui::SameLine();
		UI::KeyBindButton(&aimkey);
	}	

	if (ImGui::Checkbox(XORSTR("Aim Step"), &aimStepEnabled))
		UI::UpdateWeaponSettings();
				
	if ( aimStepEnabled )
	{
		ImGui::PushItemWidth(-1);
		if (ImGui::SliderFloat(XORSTR("##STEPMIN"), &aimStepMin, 5, 35, XORSTR("MIN : %0.0f")))
			UI::UpdateWeaponSettings();

		if (ImGui::SliderFloat(XORSTR("##STEPMAX"), &aimStepMax, (aimStepMin) + 1.0f, 35, XORSTR("MAX : %0.0f")))
			UI::UpdateWeaponSettings();
		
		ImGui::PopItemWidth();
	}

	if (ImGui::Checkbox(XORSTR("##Auto Aim"), &autoAimEnabled))
		UI::UpdateWeaponSettings();
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::SliderFloat(XORSTR("##Fov"), &LegitautoAimValue, 0, 30, XORSTR("Auto Aim FOV : %0.f")))
		UI::UpdateWeaponSettings();
	ImGui::PopItemWidth();

	ImGui::PushItemWidth(-1);
	if (ImGui::Combo(XORSTR("##AIMTARGET"), (int*)&bone, targets, IM_ARRAYSIZE(targets)))
		UI::UpdateWeaponSettings();
	ImGui::PopItemWidth();
}

void Legitbot::Recoil(){

	if (ImGui::Checkbox(XORSTR("Recoil Control"), &rcsEnabled))
		UI::UpdateWeaponSettings();
	
	if (ImGui::Checkbox(XORSTR("ALLWays On"), &rcsAlwaysOn))
		UI::UpdateWeaponSettings();
	ImGui::PushItemWidth(-1);
	{
		if (ImGui::SliderFloat(XORSTR("##RCSX"), &rcsAmountX, 0, 2, XORSTR("Pitch: %0.2f")))
			UI::UpdateWeaponSettings();
		if (ImGui::SliderFloat(XORSTR("##RCSY"), &rcsAmountY, 0, 2, XORSTR("Yaw: %0.2f")))
			UI::UpdateWeaponSettings();
	}
	ImGui::PopItemWidth();
}

void Legitbot::Semirage(){

	if (ImGui::Checkbox(XORSTR("AutoShoot"), &autoShootEnabled))
		UI::UpdateWeaponSettings();
	if (ImGui::Checkbox(XORSTR("Auto Slow"), &autoSlow))
		UI::UpdateWeaponSettings();
	if (ImGui::Checkbox(XORSTR("AutoWall"), &autowall))
		UI::UpdateWeaponSettings();
	
	switch (currentWeapon)
	{
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
			if (ImGui::Checkbox(XORSTR("Auto Pistol"), &autoPistolEnabled))
				UI::UpdateWeaponSettings();
			break;
		default:
			break;
	}

	if (ImGui::Checkbox(XORSTR("##HitChance"), &hitchanceEnaled))
		UI::UpdateWeaponSettings();
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if ( ImGui::SliderFloat(XORSTR("##HitchanceValue"), &hitchance, 0, 100, XORSTR("Hitchance: %0.f")) )
		UI::UpdateWeaponSettings();
	ImGui::PopItemWidth();

	if (ImGui::Checkbox(XORSTR("##MINDAMAGEENEBLED"), &mindamage))
		UI::UpdateWeaponSettings();
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
	if (ImGui::SliderFloat(XORSTR("##MINDAMAGE"), &mindamagevalue, 0, 100, XORSTR("Min Damage: %.0f")))
		UI::UpdateWeaponSettings();
	ImGui::PopItemWidth();
	
}

void Legitbot::Others(){
	if (ImGui::Checkbox(XORSTR("Silent Aim"), &silent))
		UI::UpdateWeaponSettings();
	if (ImGui::Checkbox(XORSTR("Prediction"), &predEnabled))
		UI::UpdateWeaponSettings();

	switch (currentWeapon)
	{
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
			if (ImGui::Checkbox(XORSTR("Auto Scope"), &autoScopeEnabled))
				UI::UpdateWeaponSettings();
	}
	
	if (ImGui::Checkbox(XORSTR("Ignore Jump (Self)"), &ignoreJumpEnabled))
		UI::UpdateWeaponSettings();
	if (ImGui::Checkbox(XORSTR("Ignore Jump (Enemies)"), &ignoreEnemyJumpEnabled))
		UI::UpdateWeaponSettings();
}

void Legitbot::Humanising(){
	static const char* smoothTypes[] = 
		{ 
			"Slow Near End", 
			"Constant Speed", 
			"Fast Near End" 
		};

	ImGui::PushItemWidth(-1);
	{
		if (ImGui::Checkbox(XORSTR("##Smoothing"), &smoothEnabled))	
			UI::UpdateWeaponSettings();
		ImGui::SameLine();
		if ( ImGui::SliderFloat(XORSTR("##SMOOTH"), &smoothValue, 0, 1, XORSTR("Smoothing : %0.2f")) )
			UI::UpdateWeaponSettings();
	}

	{
		if (ImGui::Checkbox(XORSTR("##SmoothSalting"), &smoothSaltEnabled))	
			UI::UpdateWeaponSettings();
		ImGui::SameLine();
		if (ImGui::SliderFloat(XORSTR("##SALT"), &smoothSaltMultiplier, 0, smoothValue, XORSTR("Smooth Salting : %0.2f")))
			UI::UpdateWeaponSettings();
	}	
	
	{
		if (ImGui::Checkbox(XORSTR("##ErrorMargin"), &errorMarginEnabled))	
			UI::UpdateWeaponSettings();
		ImGui::SameLine();
		if (ImGui::SliderFloat(XORSTR("##ERROR"), &errorMarginValue, 0, 2, XORSTR("Error In Shot : %0.2f")))
			UI::UpdateWeaponSettings();
	}

	{
		if (ImGui::Combo(XORSTR("##SMOOTHTYPE"), (int*)& smoothType, smoothTypes, IM_ARRAYSIZE(smoothTypes)))
			UI::UpdateWeaponSettings();
	}

	ImGui::PopItemWidth();
}

void Legitbot::RenderTab()
{
	
	static char filterWeapons[32];

	if (ImGui::Checkbox(XORSTR("Enabled"), &Settings::Legitbot::enabled))
	{	
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
		for (auto it : ItemDefinitionIndexMap)
		{
			bool isDefault = (int) it.first < 0;
			if (!isDefault && !Util::Contains(Util::ToLower(std::string(filterWeapons)), Util::ToLower(Util::Items::GetItemDisplayName(it.first).c_str())))
				continue;

			if (Util::Items::IsKnife(it.first) || Util::Items::IsGlove(it.first) || Util::Items::IsUtility(it.first))
				continue;

			const bool item_selected = ((int) it.first == (int) currentWeapon);
			ImGui::PushID((int) it.first);
			std::string formattedName;
			char changeIndicator = ' ';
			bool isChanged = Settings::Legitbot::weapons.find(it.first) != Settings::Legitbot::weapons.end();
			if (!isDefault && isChanged)
				changeIndicator = '*';
			formattedName = changeIndicator + (isDefault ? Util::Items::GetItemDisplayName(it.first).c_str() : Util::Items::GetItemDisplayName(it.first));
			if (ImGui::Selectable(formattedName.c_str(), item_selected))
			{
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
		ImGui::BeginChild(XORSTR("COL1"), ImVec2(0, 0), false);
		{

			ImGui::BeginGroupPanel(XORSTR("AimBot"));
			{
				Aimbot();

			}ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("RCS"));
			{
				Recoil();

			}ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Humanising"));
			{
				Humanising();

			}ImGui::EndGroupPanel();

		}
		ImGui::EndChild();
	}
	
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("COL2"), ImVec2(0, 0), false);
		{
			
			ImGui::BeginGroupPanel(XORSTR("SemiRage"));
			{
				Semirage();
			}ImGui::EndGroupPanel();
			
			ImGui::BeginGroupPanel(XORSTR("Others"));
			{
				Others();
			}ImGui::EndGroupPanel();

			ImGui::Spacing();

			if (currentWeapon > ItemDefinitionIndex::INVALID && Settings::Legitbot::weapons.find(currentWeapon) != Settings::Legitbot::weapons.end())
			{
				if (ImGui::Button(XORSTR("Clear Weapon Settings"), ImVec2(-1, 0)))
				{
					Settings::Legitbot::weapons.erase(currentWeapon);
					UI::ReloadWeaponSettings();
				}
			}
			ImGui::EndChild();
		}
	}
	
	ImGui::EndColumns();
	
}


