#include "visualstab.hpp"
#include "../tooltip.h"

#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wenum-compare"

enum {
	PlayerVisual,
	Radar,
	Hitmarker,
	Others,
};

static const char* BackendTypes[] = { "Surface (Valve)", "ImGUI (Custom/Faster)" };
static const char* BoxTypes[] = { "Flat 2D", "Frame 2D", "Box 3D", "Hitboxes" };
static const char* TracerTypes[] = { "Bottom", "Cursor" };
static const char* BarTypes[] = { "Vertical Left", "Vertical Right", "Horizontal Below", "Horizontal Above", "Interwebz" };	
static const char* chamsTypes[] = { 
							"ADDTIVE",
							"ADDTIVE TWO",
							"WIREFRAME",
							"FLAT",
							"PEARL",
							"GLOW",
							"GLOWF",
							"NONE",
							};
static const char* SmokeTypes[] = { "Wireframe", "None" };
static const char* SkyBoxes[] = {
		"cs_baggage_skybox_", // 0
		"cs_tibet",
		"embassy",
		"italy",
		"jungle",
		"office",
		"nukeblank",
		"sky_venice",
		"sky_cs15_daylight01_hdr",
		"sky_cs15_daylight02_hdr",
		"sky_cs15_daylight03_hdr",
		"sky_cs15_daylight04_hdr",
		"sky_csgo_cloudy01",
		"sky_csgo_night_flat",
		"sky_csgo_night02",
		"sky_csgo_night02b",
		"sky_day02_05",
		"sky_day02_05_hdr",
		"sky_dust",
		"vertigo",
		"vertigo_hdr",
		"vertigoblue_hdr",
		"vietnam" // 21
};
	

static void FilterEnemies()
{
	ImGui::PushItemWidth(-1);
		ImGui::CheckboxFill(XORSTR("##BoxEnemy"), &Settings::ESP::FilterEnemy::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPEEnemy"), (int*)& Settings::ESP::FilterEnemy::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::CheckboxFill(XORSTR("##ChamsEnenemy"), &Settings::ESP::FilterEnemy::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##CHAMSTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::CheckboxFill(XORSTR("##HealthBarEnenemy"), &Settings::ESP::FilterEnemy::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Helth Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::CheckboxFill(XORSTR("##TracersEnenemy"), &Settings::ESP::FilterEnemy::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPEEnemy"), (int*)& Settings::ESP::FilterEnemy::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));

		ImGui::CheckboxFill(XORSTR("##PlayerInfoEnemy"), &Settings::ESP::FilterEnemy::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));

		ImGui::CheckboxFill(XORSTR("##HeadDotEnemy"), &Settings::ESP::FilterEnemy::HeadDot::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Head Dot"));
		
		ImGui::CheckboxFill(XORSTR("##SkeletonEnemy"), &Settings::ESP::FilterEnemy::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));
		
		ImGui::CheckboxFill(XORSTR("##BulletBeamEnemy"), &Settings::ESP::FilterEnemy::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));

	ImGui::PopItemWidth();
}

static void FilterLocalPlayer()
{
	ImGui::PushItemWidth(-1);

		ImGui::CheckboxFill(XORSTR("##BoxLocal"), &Settings::ESP::FilterLocalPlayer::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPELOCAL"), (int*)& Settings::ESP::FilterLocalPlayer::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::CheckboxFill(XORSTR("##FakeChams"), &Settings::ESP::FilterLocalPlayer::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Fake Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##FakeCHAMSTYPE"), (int*)& Settings::ESP::FilterLocalPlayer::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::CheckboxFill(XORSTR("##RealChams"), &Settings::ESP::FilterLocalPlayer::RealChams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Real Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##RealCHAMSTYPE"), (int*)& Settings::ESP::FilterLocalPlayer::RealChams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::CheckboxFill(XORSTR("##HealthLocal"), &Settings::ESP::FilterLocalPlayer::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Helth Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPELocal"), (int*)& Settings::ESP::FilterLocalPlayer::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::CheckboxFill(XORSTR("##TracersLocal"), &Settings::ESP::FilterLocalPlayer::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPELocal"), (int*)& Settings::ESP::FilterLocalPlayer::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
	
	ImGui::PopItemWidth();
	
		ImGui::CheckboxFill(XORSTR("##PlayerInfoLocal"), &Settings::ESP::FilterLocalPlayer::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));
		ImGui::CheckboxFill(XORSTR("##SkeletonLocal"), &Settings::ESP::FilterLocalPlayer::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));

		ImGui::CheckboxFill(XORSTR("##BulletBeamLocalPlayer"), &Settings::ESP::FilterLocalPlayer::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));

}

static void FilterAlice()
{	
	ImGui::PushItemWidth(-1);
		ImGui::CheckboxFill(XORSTR("##BoxAlise"), &Settings::ESP::FilterAlice::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPEAlise"), (int*)& Settings::ESP::FilterAlice::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::CheckboxFill(XORSTR("##ChamsAlise"), &Settings::ESP::FilterAlice::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##CHAMSTYPEAlise"), (int*)& Settings::ESP::FilterAlice::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::CheckboxFill(XORSTR("##HealthAlise"), &Settings::ESP::FilterAlice::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Health Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPEAlise"), (int*)& Settings::ESP::FilterAlice::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::CheckboxFill(XORSTR("##TracersAlise"), &Settings::ESP::FilterAlice::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPEAlise"), (int*)& Settings::ESP::FilterAlice::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
	
		ImGui::PopItemWidth();
	
	
		ImGui::CheckboxFill(XORSTR("##PlayerInfoAlise"), &Settings::ESP::FilterAlice::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));

		ImGui::CheckboxFill(XORSTR("##HeadDotAlise"), &Settings::ESP::FilterAlice::HeadDot::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Head Dot"));

		ImGui::CheckboxFill(XORSTR("##SkeletonAlise"), &Settings::ESP::FilterAlice::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));

		ImGui::CheckboxFill(XORSTR("##BulletBeamAlise"), &Settings::ESP::FilterAlice::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));
	
}

void Visuals::Visibility(){
	ImGui::CheckboxFill(XORSTR("Smoke Check"), &Settings::ESP::Filters::smokeCheck);
	ImGui::CheckboxFill(XORSTR("Legit Mode"), &Settings::ESP::Filters::legit);
	ImGui::CheckboxFill(XORSTR("Visibility Check"), &Settings::ESP::Filters::visibilityCheck);			
}

void Visuals::PlayerDetails(){
		ImGui::CheckboxFill(XORSTR("Clan"), &Settings::ESP::Info::clan);
		ImGui::CheckboxFill(XORSTR("Rank"), &Settings::ESP::Info::rank);
		ImGui::CheckboxFill(XORSTR("Health"), &Settings::ESP::Info::health);
		ImGui::CheckboxFill(XORSTR("Armor"), &Settings::ESP::Info::armor);
		ImGui::CheckboxFill(XORSTR("Scoped"), &Settings::ESP::Info::scoped);
		ImGui::CheckboxFill(XORSTR("Flashed"), &Settings::ESP::Info::flashed);
		ImGui::CheckboxFill(XORSTR("Defuse Kit"), &Settings::ESP::Info::hasDefuser);
		ImGui::CheckboxFill(XORSTR("Grabbing Hostage"), &Settings::ESP::Info::grabbingHostage);
		ImGui::CheckboxFill(XORSTR("Location"), &Settings::ESP::Info::location);
		ImGui::CheckboxFill(XORSTR("Name"), &Settings::ESP::Info::name);
		ImGui::CheckboxFill(XORSTR("Steam ID"), &Settings::ESP::Info::steamId);
		ImGui::CheckboxFill(XORSTR("Weapon"), &Settings::ESP::Info::weapon);
		ImGui::CheckboxFill(XORSTR("Reloading"), &Settings::ESP::Info::reloading);
		ImGui::CheckboxFill(XORSTR("Planting"), &Settings::ESP::Info::planting);
		ImGui::CheckboxFill(XORSTR("Defusing"), &Settings::ESP::Info::defusing);
		ImGui::CheckboxFill(XORSTR("Rescuing Hostage"), &Settings::ESP::Info::rescuing);
        ImGui::CheckboxFill(XORSTR("Layers Debug"), &Settings::Debug::AnimLayers::draw);
		ImGui::CheckboxFill(XORSTR("Money"), &Settings::ESP::Info::money);
}

void Visuals::DangerZone(){

	ImGui::CheckboxFill(XORSTR("Loot Crates"), &Settings::ESP::DangerZone::lootcrate);
	ImGui::CheckboxFill(XORSTR("Weapon Upgrades"), &Settings::ESP::DangerZone::upgrade);
	ImGui::CheckboxFill(XORSTR("Ammo box"), &Settings::ESP::DangerZone::ammobox);
	ImGui::CheckboxFill(XORSTR("Radar Jammer"), &Settings::ESP::DangerZone::radarjammer);
	ImGui::CheckboxFill(XORSTR("Cash"), &Settings::ESP::DangerZone::cash);
	ImGui::CheckboxFill(XORSTR("Drone"), &Settings::ESP::DangerZone::drone);
	ImGui::CheckboxFill(XORSTR("Draw Distance"), &Settings::ESP::DangerZone::drawDistEnabled);
	ImGui::CheckboxFill(XORSTR("Safe"), &Settings::ESP::DangerZone::safe);
	ImGui::CheckboxFill(XORSTR("Sentry Turret"), &Settings::ESP::DangerZone::dronegun);
	ImGui::CheckboxFill(XORSTR("Melee"), &Settings::ESP::DangerZone::melee);
	ImGui::CheckboxFill(XORSTR("Tablet"), &Settings::ESP::DangerZone::tablet);
	ImGui::CheckboxFill(XORSTR("Healthshot"), &Settings::ESP::DangerZone::healthshot);
	ImGui::CheckboxFill(XORSTR("Explosive Barrel"), &Settings::ESP::DangerZone::barrel);
	if (Settings::ESP::DangerZone::drawDistEnabled)
		ImGui::SliderInt(XORSTR("##DZDRAWDIST"), &Settings::ESP::DangerZone::drawDist, 1, 10000, XORSTR("Amount: %0.f"));
}

void Visuals::Items(){
	ImGui::CheckboxFill(XORSTR("Weapons"), &Settings::ESP::Filters::weapons);
	ImGui::CheckboxFill(XORSTR("Throwables"), &Settings::ESP::Filters::throwables);
	ImGui::CheckboxFill(XORSTR("Bomb"), &Settings::ESP::Filters::bomb);
	ImGui::CheckboxFill(XORSTR("Defuse Kits"), &Settings::ESP::Filters::defusers);
	ImGui::CheckboxFill(XORSTR("Hostages"), &Settings::ESP::Filters::hostages);
			
}

void Visuals::Others(){
	ImGui::PushItemWidth(-1);

	ImGui::CheckboxFill(XORSTR("Skybox"), &Settings::SkyBox::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##SKYBOX"), &Settings::SkyBox::skyBoxNumber, SkyBoxes, IM_ARRAYSIZE(SkyBoxes));
				
	ImGui::CheckboxFill(XORSTR("Arms"), &Settings::ESP::Chams::Arms::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##ARMSTYPE"), (int*)& Settings::ESP::Chams::Arms::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				
	ImGui::CheckboxFill(XORSTR("Weapons"), &Settings::ESP::Chams::Weapon::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##WEAPONTYPE"), (int*)& Settings::ESP::Chams::Weapon::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				
	ImGui::CheckboxFill(XORSTR("Dlights"), &Settings::Dlights::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##DLIGHTRADIUS"), &Settings::Dlights::radius, 0, 1000, XORSTR("Radius: %0.f"));
				
	ImGui::CheckboxFill(XORSTR("No Flash"), &Settings::Noflash::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##NOFLASHAMOUNT"), &Settings::Noflash::value, 0, 255, XORSTR("Amount: %0.f"));
				
	ImGui::CheckboxFill(XORSTR("No Smoke"), &Settings::NoSmoke::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##SMOKETYPE"), (int*)& Settings::NoSmoke::type, SmokeTypes, IM_ARRAYSIZE(SmokeTypes));
	
	ImGui::CheckboxFill(XORSTR("Footsteps"), &Settings::ESP::Sounds::enabled);
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##SOUNDSTIME"), &Settings::ESP::Sounds::time, 250, 5000, XORSTR("Timeout: %0.f"));	
	
	ImGui::PopItemWidth();
	
	ImGui::CheckboxFill(XORSTR("No Sky"), &Settings::NoSky::enabled);
	ImGui::CheckboxFill(XORSTR("ASUS Walls"), &Settings::ASUSWalls::enabled);
	ImGui::SliderInt(XORSTR("Test Model ID"), &Settings::Debug::BoneMap::modelID, 1253, 1350, XORSTR("Model ID: %0.f") );
}

void Visuals::Supportive(){
	ImGui::CheckboxFill( XORSTR( "Grenade Prediction" ), &Settings::GrenadePrediction::enabled );
	ImGui::CheckboxFill(XORSTR("Autowall Debug"), &Settings::Debug::AutoWall::debugView);
	ImGui::CheckboxFill(XORSTR("AimSpot Debug"), &Settings::Debug::AutoAim::drawTarget);
	ImGui::CheckboxFill(XORSTR("No Aim Punch"), &Settings::View::NoAimPunch::enabled);
	ImGui::CheckboxFill(XORSTR("No View Punch"), &Settings::View::NoViewPunch::enabled);
	ImGui::CheckboxFill(XORSTR("No Scope Border"), &Settings::NoScopeBorder::enabled);
	ImGui::CheckboxFill(XORSTR("BoneMap Debug"), &Settings::Debug::BoneMap::draw);
	if( Settings::Debug::BoneMap::draw )
		ImGui::CheckboxFill(XORSTR("Just Dots"), &Settings::Debug::BoneMap::justDrawDots);
}

static void CrosshAir(){
	ImGui::PushItemWidth(-1);
	ImGui::CheckboxFill(XORSTR("Recoil Crosshair"), &Settings::Recoilcrosshair::enabled);
	ImGui::CheckboxFill(XORSTR("FOV Circle"), &Settings::ESP::FOVCrosshair::enabled);
	ImGui::CheckboxFill(XORSTR("Show Spread"), &Settings::ESP::Spread::enabled);
	ImGui::CheckboxFill(XORSTR("Only When Shooting"), &Settings::Recoilcrosshair::showOnlyWhenShooting);
	ImGui::CheckboxFill(XORSTR("Filled"), &Settings::ESP::FOVCrosshair::filled);
	ImGui::CheckboxFill(XORSTR("Show SpreadLimit"), &Settings::ESP::Spread::spreadLimit);
	ImGui::PopItemWidth();
}

void Visuals::RenderTab()
{
	// Backend For Visuals
    ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
    ImGui::Combo( XORSTR( "##BACKENDTYPE" ), (int*)&Settings::ESP::backend, BackendTypes, IM_ARRAYSIZE( BackendTypes ) );
    ImGui::PopItemWidth();

	// Filter Visibility
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(-1, -1), false);
		{
			ImGui::BeginGroupPanel(XORSTR("Enemy"));
			{
				FilterEnemies();
			}ImGui::EndGroupPanel2();

			ImGui::BeginGroupPanel(XORSTR("LocalPlayer"));
			{
				FilterLocalPlayer();
			}ImGui::EndGroupPanel2();

			ImGui::BeginGroupPanel(XORSTR("Alise"));
			{
				FilterAlice();
			}ImGui::EndGroupPanel2();


			ImGui::PushItemWidth(-1);
		

			if ( ImGui::BeginCombo(XORSTR("##CrossHairOptions"), XORSTR("Crosshair Options")) )
			{
				ImGui::Columns(2, nullptr, false);
				{
					ImGui::Selectable(XORSTR("Recoil Crosshair"), &Settings::Recoilcrosshair::enabled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("FOV Circle"), &Settings::ESP::FOVCrosshair::enabled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Show Spread"), &Settings::ESP::Spread::enabled, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::NextColumn();
				{
					ImGui::Selectable(XORSTR("Only When Shooting"), &Settings::Recoilcrosshair::showOnlyWhenShooting, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Filled"), &Settings::ESP::FOVCrosshair::filled, ImGuiSelectableFlags_DontClosePopups);
					ImGui::Selectable(XORSTR("Show SpreadLimit"), &Settings::ESP::Spread::spreadLimit, ImGuiSelectableFlags_DontClosePopups);
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			ImGui::EndChild();
		}
	}
	ImGui::NextColumn();
	{
		ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 0), false);
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::BeginGroupPanel(XORSTR("Visibility"));
				{
					Visibility();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Supportive"));
				{
					Supportive();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Player Info"));
				{
					PlayerDetails();
				}ImGui::EndGroupPanel();

				

			}ImGui::NextColumn();
			{
				ImGui::BeginGroupPanel(XORSTR("World Items"));
				{
					Items();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Others"));
				{
					Others();
				}ImGui::EndGroupPanel();
				
				ImGui::BeginGroupPanel(XORSTR("Danger Zone"));
				{
					DangerZone();
				}ImGui::EndGroupPanel();

			}
			ImGui::EndColumns();
			

				if ( ImGui::Button( XORSTR( "Material Config" ), ImVec2( -1, 0 ) ) )
					ImGui::OpenPopup( XORSTR( "##MaterialConfigWindow" ) );
				SetTooltip( XORSTR( "Advanced CSGO Gfx Settings\nExperimental" ) );
				ImGui::SetNextWindowSize( ImVec2( 320, 640 ), ImGuiSetCond_Always );
				if ( ImGui::BeginPopup( XORSTR( "##MaterialConfigWindow" ) ) ) {
					ImGui::PushItemWidth( -1 );
					if ( ImGui::Button( XORSTR( "Reset Changes" ) ) ) {
						Settings::MaterialConfig::config = MaterialConfig::backupConfig;
					}
					ImGui::CheckboxFill( XORSTR( "Changes Enabled?" ), &Settings::MaterialConfig::enabled );
					SetTooltip( XORSTR( "Expect some lag when changing these settings.\nIf your hud breaks, toggle cl_drawhud off/on" ) );
					ImGui::SliderFloat( XORSTR( "##MONITORGAMMA" ), &Settings::MaterialConfig::config.m_fMonitorGamma, 0.1f, 12.0f,
                                        XORSTR( "Gamma: %.3f" ) );
					ImGui::SliderFloat( XORSTR( "##GAMMATVRANGEMIN" ), &Settings::MaterialConfig::config.m_fGammaTVRangeMin,
										0.1f, std::min( 300.0f, Settings::MaterialConfig::config.m_fGammaTVRangeMax ),
										XORSTR( "TVRangeMin: %.3f" ) );
					ImGui::SliderFloat( XORSTR( "##GAMMATVRANGEMAX" ), &Settings::MaterialConfig::config.m_fGammaTVRangeMax,
										Settings::MaterialConfig::config.m_fGammaTVRangeMin, 300.0f,
										XORSTR( "TVRangeMax: %.3f" ) );
					ImGui::SliderFloat( XORSTR( "##GAMMATVEXPONENT" ), &Settings::MaterialConfig::config.m_fGammaTVExponent,
										0.1f, 3.0f, XORSTR( "TV Exponent: %.3f" ) );
					ImGui::CheckboxFill( XORSTR( "GammaTVEnabled" ), &Settings::MaterialConfig::config.m_bGammaTVEnabled );
					ImGui::Text( XORSTR( "Width:" ) );
					ImGui::SameLine();
					ImGui::InputInt( XORSTR( "##GAMEWIDTH" ), &Settings::MaterialConfig::config.m_VideoMode.m_Width );

					ImGui::Text( XORSTR( "Height:" ) );
					ImGui::SameLine();
					ImGui::InputInt( XORSTR( "##GAMEHEIGHT" ), &Settings::MaterialConfig::config.m_VideoMode.m_Height );

					ImGui::Text( XORSTR( "Refresh Rate:" ) );
					ImGui::SameLine();
					ImGui::InputInt( XORSTR( "##GAMEREFRESHRATE" ),
									 &Settings::MaterialConfig::config.m_VideoMode.m_RefreshRate );

					ImGui::CheckboxFill( XORSTR( "TripleBuffered" ), &Settings::MaterialConfig::config.m_bTripleBuffered );
					ImGui::SliderInt( XORSTR( "##AASAMPLES" ), &Settings::MaterialConfig::config.m_nAASamples, 0, 16, XORSTR( "AA Samples: %1.f" ) );
					ImGui::SliderInt( XORSTR( "##FORCEANISOTROPICLEVEL" ), &Settings::MaterialConfig::config.m_nForceAnisotropicLevel, 0, 8, XORSTR( "Anisotropic Level: %1.f" ) );
					ImGui::SliderInt( XORSTR( "##SKIPMIPLEVELS" ), &Settings::MaterialConfig::config.skipMipLevels, 0, 10, XORSTR( "SkipMipLevels: %1.f" ) );
					SetTooltip( XORSTR( "Makes the game flatter.") );
					if ( ImGui::Button( XORSTR( "Flags" ), ImVec2( -1, 0 ) ) )
						ImGui::OpenPopup( XORSTR( "##MaterialConfigFlags" ) );
					ImGui::SetNextWindowSize( ImVec2( 320, 240 ), ImGuiSetCond_Always );
					if ( ImGui::BeginPopup( XORSTR( "##MaterialConfigFlags" ) ) ) {
						ImGui::PushItemWidth( -1 );

						static bool localFlags[] = {
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_WINDOWED ) != 0, // ( 1 << 0 )
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_RESIZING ) != 0, // ( 1 << 1 )
								false, // ( 1 << 2 ) is not used.
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC ) != 0, // ( 1 << 3 )
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_STENCIL ) != 0, // ...
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_ENABLE_HDR ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_DISABLE_PHONG ) != 0,
								( Settings::MaterialConfig::config.m_Flags & MaterialSystem_Config_Flags_t::MATSYS_VIDCFG_FLAGS_VR_MODE ) != 0
						};

						ImGui::CheckboxFill( XORSTR( "Windowed" ), &localFlags[0] );
						ImGui::CheckboxFill( XORSTR( "Resizing" ), &localFlags[1] );
						ImGui::CheckboxFill( XORSTR( "No VSYNC Wait" ), &localFlags[3] );
						ImGui::CheckboxFill( XORSTR( "Stencil" ), &localFlags[4] );
						ImGui::CheckboxFill( XORSTR( "Force Tri-Linear" ), &localFlags[5] );
						ImGui::CheckboxFill( XORSTR( "Force HW Sync" ), &localFlags[6] );
						ImGui::CheckboxFill( XORSTR( "Disable Specular" ), &localFlags[7] );
						ImGui::CheckboxFill( XORSTR( "Disable Bumpmap" ), &localFlags[8] );
						ImGui::CheckboxFill( XORSTR( "Disable Phong" ), &localFlags[16] );
						ImGui::CheckboxFill( XORSTR( "Parallax Mapping" ), &localFlags[9] );
						ImGui::CheckboxFill( XORSTR( "Use Z-Prefill" ), &localFlags[10] );
						ImGui::CheckboxFill( XORSTR( "Reduce FillRate" ), &localFlags[11] );
						ImGui::CheckboxFill( XORSTR( "HDR" ), &localFlags[12] );
						ImGui::CheckboxFill( XORSTR( "Limit Windowed Size" ), &localFlags[13] );
						ImGui::CheckboxFill( XORSTR( "Scale to Output Resolution" ), &localFlags[14] );
						ImGui::CheckboxFill( XORSTR( "Using Multiple Windows" ), &localFlags[15] );
						ImGui::CheckboxFill( XORSTR( "VR-Mode" ), &localFlags[17] );

						if ( ImGui::Button( XORSTR( "Apply " ) ) ) {
							for ( unsigned short i = 0; i < 18; i++ ) {
								if ( i == 2 ) // ( 1 << 2 ) not used.
									continue;
								Settings::MaterialConfig::config.SetFlag( ( unsigned int ) ( 1 << i ), localFlags[i] );
							}
						}

						ImGui::PopItemWidth();
						ImGui::EndPopup();
					}
					//m_flags
					ImGui::CheckboxFill( XORSTR( "EditMode" ), &Settings::MaterialConfig::config.bEditMode );
					//proxiesTestMode
					ImGui::CheckboxFill( XORSTR ( "Compressed Textures" ),
									 &Settings::MaterialConfig::config.bCompressedTextures );
					ImGui::CheckboxFill( XORSTR( "Filter Lightmaps" ), &Settings::MaterialConfig::config.bFilterLightmaps );
					ImGui::CheckboxFill( XORSTR( "Filter Textures" ), &Settings::MaterialConfig::config.bFilterTextures );
					ImGui::CheckboxFill( XORSTR( "Reverse Depth" ), &Settings::MaterialConfig::config.bReverseDepth );
					ImGui::CheckboxFill( XORSTR( "Buffer Primitives" ), &Settings::MaterialConfig::config.bBufferPrimitives );
					ImGui::CheckboxFill( XORSTR( "Draw Flat" ), &Settings::MaterialConfig::config.bDrawFlat );
					ImGui::CheckboxFill( XORSTR( "Measure Fill-Rate" ), &Settings::MaterialConfig::config.bMeasureFillRate );
					ImGui::CheckboxFill( XORSTR( "Visualize Fill-Rate" ),
									 &Settings::MaterialConfig::config.bVisualizeFillRate );
					ImGui::CheckboxFill( XORSTR( "No Transparency" ), &Settings::MaterialConfig::config.bNoTransparency );
					ImGui::CheckboxFill( XORSTR( "Software Lighting" ),
									 &Settings::MaterialConfig::config.bSoftwareLighting ); // Crashes game
					//AllowCheats ?
					ImGui::SliderInt( XORSTR( "##MIPLEVELS" ), ( int* ) &Settings::MaterialConfig::config.nShowMipLevels, 0,
									  3, XORSTR( "ShowMipLevels: %1.f" ) );
					ImGui::CheckboxFill( XORSTR( "Show Low-Res Image" ), &Settings::MaterialConfig::config.bShowLowResImage );
					ImGui::CheckboxFill( XORSTR( "Show Normal Map" ), &Settings::MaterialConfig::config.bShowNormalMap );
					ImGui::CheckboxFill( XORSTR( "MipMap Textures" ), &Settings::MaterialConfig::config.bMipMapTextures );
					ImGui::SliderInt( XORSTR( "##NFULLBRIGHT" ), ( int* ) &Settings::MaterialConfig::config.nFullbright, 0, 3, XORSTR( "nFullBright: %1.f" ) );
					SetTooltip( XORSTR( "1 = Bright World, 2 = Bright Models" ) );
					ImGui::CheckboxFill( XORSTR( "Fast NoBump" ), &Settings::MaterialConfig::config.m_bFastNoBump );
					ImGui::CheckboxFill( XORSTR( "Suppress Rendering" ),
									 &Settings::MaterialConfig::config.m_bSuppressRendering );
					ImGui::CheckboxFill( XORSTR( "Draw Gray" ), &Settings::MaterialConfig::config.m_bDrawGray );
					ImGui::CheckboxFill( XORSTR( "Show Specular" ), &Settings::MaterialConfig::config.bShowSpecular );
					ImGui::CheckboxFill( XORSTR( "Show Defuse" ), &Settings::MaterialConfig::config.bShowDiffuse );
					ImGui::SliderInt( XORSTR( "##AAQUALITY" ), &Settings::MaterialConfig::config.m_nAAQuality, 0, 16,
									  XORSTR( "AAQuality: %1.f" ) );
					ImGui::CheckboxFill( XORSTR( "Shadow Depth Texture" ),
									 &Settings::MaterialConfig::config.m_bShadowDepthTexture );
					SetTooltip( XORSTR( "Risky. May cause black Screen. Reset if it does." ) );
					ImGui::CheckboxFill( XORSTR( "Motion Blur" ), &Settings::MaterialConfig::config.m_bMotionBlur );
					ImGui::CheckboxFill( XORSTR( "Support Flashlight" ),
									 &Settings::MaterialConfig::config.m_bSupportFlashlight );
					ImGui::CheckboxFill( XORSTR( "Paint Enabled" ), &Settings::MaterialConfig::config.m_bPaintEnabled );
					// VRMode Adapter?
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
		}
		ImGui::EndChild();
	}
}

void Visuals::RenderAimware(ImVec2 &pos, ImDrawList * draw, int sideTabIndex){

	draw->AddRectFilled(ImVec2(pos.x + 180, pos.y + 65), ImVec2(pos.x + 945, pos.y + 95), ImColor(0, 0, 0, 150), 10);
    ImGui::SetCursorPos(ImVec2(185, 70));
	ImGui::BeginGroup();
    {
        ImGui::CheckboxFill(XORSTR("Enabled"), &Settings::ESP::enabled);
    }ImGui::EndGroup();
    ToolTip::Show(XORSTR("Enable Visual"), ImGui::IsItemHovered());

	if ( !Settings::ESP::enabled )
		goto DoNotRender;
	
	ImGui::SetCursorPos(ImVec2(180, 100));
	if ( sideTabIndex == PlayerVisual){
		ImGui::BeginGroup();
		{	
			ImGui::Columns(3, nullptr, false);
			{
				ImGui::BeginChild(XORSTR("##PlayerVisuals1"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Player Info"));
					{
						PlayerDetails();
					}ImGui::EndGroupPanel();
				}ImGui::EndChild();

			}ImGui::NextColumn();
			{
				ImGui::BeginChild(XORSTR("##PlayerVisuals2"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Enemy"));
					{
						FilterEnemies();
					}ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Visibility"));
					{
						Visibility();
					}ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("CrosshAir"));
					{
						CrosshAir();
					}ImGui::EndGroupPanel();


				}ImGui::EndChild();

			}ImGui::NextColumn();
			{	
				ImGui::BeginChild(XORSTR("##PlayerVisuals3"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("LocalPlayer"));
					{
						FilterLocalPlayer();
					}ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Alise"));
					{
						FilterAlice();
					}ImGui::EndGroupPanel();

				}ImGui::EndChild();

			}ImGui::EndColumns();

		}ImGui::EndGroup();

	}
	else if ( sideTabIndex == Radar ) {
		ImGui::BeginGroup();
		{
			ImGui::Columns();
			{
				ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Hit Marker"));
					{
						Other::Hitmarkers();
					}ImGui::EndGroupPanel();
					
				}ImGui::EndChild();
			}
		}ImGui::EndGroup();
	}
	else if ( sideTabIndex == Hitmarker ){
		ImGui::BeginGroup();
		{
			ImGui::Columns();
			{
				ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(0, 0), false);
				{
					ImGui::BeginGroupPanel(XORSTR("Radar"));
					{
						Other::RadarOptions();
					}ImGui::EndGroupPanel();
					
				}ImGui::EndChild();
			}
		}ImGui::EndGroup();
	}
	else {

		ImGui::BeginGroup();
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::BeginChild(XORSTR("##Visuals1"), ImVec2(0, 0), false);
				{

					ImGui::BeginGroupPanel(XORSTR("Supportive"));
					{
						Supportive();
					}ImGui::EndGroupPanel();

					ImGui::BeginGroupPanel(XORSTR("Others"));
					{
						Others();
					}ImGui::EndGroupPanel();

				}ImGui::EndChild();
			}
			ImGui::NextColumn();
			{
				ImGui::BeginChild(XORSTR("##Visuals2"), ImVec2(0, 0), false);
				{
					
					ImGui::BeginGroupPanel(XORSTR("World Items"));
					{
						Items();
					}ImGui::EndGroupPanel();
					
					ImGui::BeginGroupPanel(XORSTR("Danger Zone"));
					{
						DangerZone();
					}ImGui::EndGroupPanel();


				}ImGui::EndChild();

			}ImGui::EndColumns();

		}ImGui::EndGroup();
	
	}

	DoNotRender:
	;
}

