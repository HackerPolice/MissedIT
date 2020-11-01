#include "visualstab.hpp"

#pragma GCC diagnostic ignored "-Wformat-security"
#pragma GCC diagnostic ignored "-Wenum-compare"

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
		ImGui::Checkbox(XORSTR("##BoxEnemy"), &Settings::ESP::FilterEnemy::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::Checkbox(XORSTR("##ChamsEnenemy"), &Settings::ESP::FilterEnemy::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##CHAMSTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::Checkbox(XORSTR("##HealthBarEnenemy"), &Settings::ESP::FilterEnemy::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Helth Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::Checkbox(XORSTR("##TracersEnenemy"), &Settings::ESP::FilterEnemy::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPEEnenemy"), (int*)& Settings::ESP::FilterEnemy::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));

		ImGui::Checkbox(XORSTR("##PlayerInfoEnenemy"), &Settings::ESP::FilterEnemy::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));

		ImGui::Checkbox(XORSTR("##HeadDotEnenemy"), &Settings::ESP::FilterEnemy::HeadDot::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Head Dot"));
		
		ImGui::Checkbox(XORSTR("##SkeletonEnenemy"), &Settings::ESP::FilterEnemy::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));
		
		ImGui::Checkbox(XORSTR("##BulletBeamEnemy"), &Settings::ESP::FilterEnemy::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));

	ImGui::PopItemWidth();
}

static void FilterLocalPlayer()
{
	ImGui::PushItemWidth(-1);

		ImGui::Checkbox(XORSTR("##BoxLocal"), &Settings::ESP::FilterLocalPlayer::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPELOCAL"), (int*)& Settings::ESP::FilterLocalPlayer::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::Checkbox(XORSTR("##FakeChams"), &Settings::ESP::FilterLocalPlayer::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Fake Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##FakeCHAMSTYPE"), (int*)& Settings::ESP::FilterLocalPlayer::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::Checkbox(XORSTR("##RealChams"), &Settings::ESP::FilterLocalPlayer::RealChams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Real Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##RealCHAMSTYPE"), (int*)& Settings::ESP::FilterLocalPlayer::RealChams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::Checkbox(XORSTR("##HealthLocal"), &Settings::ESP::FilterLocalPlayer::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Helth Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPELocal"), (int*)& Settings::ESP::FilterLocalPlayer::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::Checkbox(XORSTR("##TracersLocal"), &Settings::ESP::FilterLocalPlayer::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPELocal"), (int*)& Settings::ESP::FilterLocalPlayer::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
	
	ImGui::PopItemWidth();
	
		ImGui::Checkbox(XORSTR("##PlayerInfoLocal"), &Settings::ESP::FilterLocalPlayer::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));
		ImGui::Checkbox(XORSTR("##SkeletonLocal"), &Settings::ESP::FilterLocalPlayer::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));

		ImGui::Checkbox(XORSTR("##BulletBeamLocalPlayer"), &Settings::ESP::FilterLocalPlayer::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));

}

static void FilterAlise()
{	
	ImGui::PushItemWidth(-1);
		ImGui::Checkbox(XORSTR("##BoxAlise"), &Settings::ESP::FilterAlise::Boxes::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Box"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BOXTYPEAlise"), (int*)& Settings::ESP::FilterAlise::Boxes::type, BoxTypes, IM_ARRAYSIZE(BoxTypes));
		
		ImGui::Checkbox(XORSTR("##ChamsAlise"), &Settings::ESP::FilterAlise::Chams::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Chams"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##CHAMSTYPEAlise"), (int*)& Settings::ESP::FilterAlise::Chams::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
		
		ImGui::Checkbox(XORSTR("##HealthAlise"), &Settings::ESP::FilterAlise::HelthBar::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Health Bar"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##BARTYPEAlise"), (int*)& Settings::ESP::FilterAlise::HelthBar::type, BarTypes, IM_ARRAYSIZE(BarTypes));
		
		ImGui::Checkbox(XORSTR("##TracersAlise"), &Settings::ESP::FilterAlise::Tracers::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Tracers"));
		ImGui::SameLine();
		ImGui::Combo(XORSTR("##TRACERTYPEAlise"), (int*)& Settings::ESP::FilterAlise::Tracers::type, TracerTypes, IM_ARRAYSIZE(TracerTypes));
	
		ImGui::PopItemWidth();
	
	
		ImGui::Checkbox(XORSTR("##PlayerInfoAlise"), &Settings::ESP::FilterAlise::playerInfo::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Player Info"));

		ImGui::Checkbox(XORSTR("##HeadDotAlise"), &Settings::ESP::FilterAlise::HeadDot::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Head Dot"));

		ImGui::Checkbox(XORSTR("##SkeletonAlise"), &Settings::ESP::FilterAlise::Skeleton::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Skeleton"));

		ImGui::Checkbox(XORSTR("##BulletBeamAlise"), &Settings::ESP::FilterAlise::BulletBeam::enabled);
		ImGui::SameLine();
		ImGui::Text(XORSTR("Bullet Beam"));
	
}

void Visuals::Visibility(){
	ImGui::Checkbox(XORSTR("Smoke Chekc"), &Settings::ESP::Filters::smokeCheck);
	ImGui::Checkbox(XORSTR("Legit Mode"), &Settings::ESP::Filters::legit);
	ImGui::Checkbox(XORSTR("Visibility Check"), &Settings::ESP::Filters::visibilityCheck);			
}

void Visuals::PlayerDetails(){
		ImGui::Checkbox(XORSTR("Clan"), &Settings::ESP::Info::clan);
		ImGui::Checkbox(XORSTR("Rank"), &Settings::ESP::Info::rank);
		ImGui::Checkbox(XORSTR("Health"), &Settings::ESP::Info::health);
		ImGui::Checkbox(XORSTR("Armor"), &Settings::ESP::Info::armor);
		ImGui::Checkbox(XORSTR("Scoped"), &Settings::ESP::Info::scoped);
		ImGui::Checkbox(XORSTR("Flashed"), &Settings::ESP::Info::flashed);
		ImGui::Checkbox(XORSTR("Defuse Kit"), &Settings::ESP::Info::hasDefuser);
		ImGui::Checkbox(XORSTR("Grabbing Hostage"), &Settings::ESP::Info::grabbingHostage);
		ImGui::Checkbox(XORSTR("Location"), &Settings::ESP::Info::location);
		ImGui::Checkbox(XORSTR("Name"), &Settings::ESP::Info::name);
		ImGui::Checkbox(XORSTR("Steam ID"), &Settings::ESP::Info::steamId);
		ImGui::Checkbox(XORSTR("Weapon"), &Settings::ESP::Info::weapon);
		ImGui::Checkbox(XORSTR("Reloading"), &Settings::ESP::Info::reloading);
		ImGui::Checkbox(XORSTR("Planting"), &Settings::ESP::Info::planting);
		ImGui::Checkbox(XORSTR("Defusing"), &Settings::ESP::Info::defusing);
		ImGui::Checkbox(XORSTR("Rescuing Hostage"), &Settings::ESP::Info::rescuing);
        ImGui::Checkbox(XORSTR("Layers Debug"), &Settings::Debug::AnimLayers::draw);
		ImGui::Checkbox(XORSTR("Money"), &Settings::ESP::Info::money);
}

void Visuals::DangerZone(){

	ImGui::Checkbox(XORSTR("Loot Crates"), &Settings::ESP::DangerZone::lootcrate);
	ImGui::Checkbox(XORSTR("Weapon Upgrades"), &Settings::ESP::DangerZone::upgrade);
	ImGui::Checkbox(XORSTR("Ammo box"), &Settings::ESP::DangerZone::ammobox);
	ImGui::Checkbox(XORSTR("Radar Jammer"), &Settings::ESP::DangerZone::radarjammer);
	ImGui::Checkbox(XORSTR("Cash"), &Settings::ESP::DangerZone::cash);
	ImGui::Checkbox(XORSTR("Drone"), &Settings::ESP::DangerZone::drone);
	ImGui::Checkbox(XORSTR("Draw Distance"), &Settings::ESP::DangerZone::drawDistEnabled);
	ImGui::Checkbox(XORSTR("Safe"), &Settings::ESP::DangerZone::safe);
	ImGui::Checkbox(XORSTR("Sentry Turret"), &Settings::ESP::DangerZone::dronegun);
	ImGui::Checkbox(XORSTR("Melee"), &Settings::ESP::DangerZone::melee);
	ImGui::Checkbox(XORSTR("Tablet"), &Settings::ESP::DangerZone::tablet);
	ImGui::Checkbox(XORSTR("Healthshot"), &Settings::ESP::DangerZone::healthshot);
	ImGui::Checkbox(XORSTR("Explosive Barrel"), &Settings::ESP::DangerZone::barrel);
	if (Settings::ESP::DangerZone::drawDistEnabled)
		ImGui::SliderInt(XORSTR("##DZDRAWDIST"), &Settings::ESP::DangerZone::drawDist, 1, 10000, XORSTR("Amount: %0.f"));
}

void Visuals::Items(){
	ImGui::Checkbox(XORSTR("Weapons"), &Settings::ESP::Filters::weapons);
	ImGui::Checkbox(XORSTR("Throwables"), &Settings::ESP::Filters::throwables);
	ImGui::Checkbox(XORSTR("Bomb"), &Settings::ESP::Filters::bomb);
	ImGui::Checkbox(XORSTR("Defuse Kits"), &Settings::ESP::Filters::defusers);
	ImGui::Checkbox(XORSTR("Hostages"), &Settings::ESP::Filters::hostages);
			
}

void Visuals::Others(){
	ImGui::PushItemWidth(-1);

	ImGui::Checkbox(XORSTR("Skybox"), &Settings::SkyBox::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##SKYBOX"), &Settings::SkyBox::skyBoxNumber, SkyBoxes, IM_ARRAYSIZE(SkyBoxes));
				
	ImGui::Checkbox(XORSTR("Arms"), &Settings::ESP::Chams::Arms::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##ARMSTYPE"), (int*)& Settings::ESP::Chams::Arms::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				
	ImGui::Checkbox(XORSTR("Weapons"), &Settings::ESP::Chams::Weapon::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##WEAPONTYPE"), (int*)& Settings::ESP::Chams::Weapon::type, chamsTypes, IM_ARRAYSIZE(chamsTypes));
				
	ImGui::Checkbox(XORSTR("Dlights"), &Settings::Dlights::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##DLIGHTRADIUS"), &Settings::Dlights::radius, 0, 1000, XORSTR("Radius: %0.f"));
				
	ImGui::Checkbox(XORSTR("No Flash"), &Settings::Noflash::enabled);
	ImGui::SameLine();
	ImGui::SliderFloat(XORSTR("##NOFLASHAMOUNT"), &Settings::Noflash::value, 0, 255, XORSTR("Amount: %0.f"));
				
	ImGui::Checkbox(XORSTR("No Smoke"), &Settings::NoSmoke::enabled);
	ImGui::SameLine();
	ImGui::Combo(XORSTR("##SMOKETYPE"), (int*)& Settings::NoSmoke::type, SmokeTypes, IM_ARRAYSIZE(SmokeTypes));
	
	ImGui::Checkbox(XORSTR("Footsteps"), &Settings::ESP::Sounds::enabled);
	ImGui::SameLine();
	ImGui::SliderInt(XORSTR("##SOUNDSTIME"), &Settings::ESP::Sounds::time, 250, 5000, XORSTR("Timeout: %0.f"));	
	
	ImGui::PopItemWidth();
	
	ImGui::Checkbox(XORSTR("No Sky"), &Settings::NoSky::enabled);
	ImGui::Checkbox(XORSTR("ASUS Walls"), &Settings::ASUSWalls::enabled);
	ImGui::SliderInt(XORSTR("Test Model ID"), &Settings::Debug::BoneMap::modelID, 1253, 1350, XORSTR("Model ID: %0.f") );
}

void Visuals::Supportive(){
	ImGui::Checkbox( XORSTR( "Grenade Prediction" ), &Settings::GrenadePrediction::enabled );
	ImGui::Checkbox(XORSTR("Autowall Debug"), &Settings::Debug::AutoWall::debugView);
	ImGui::Checkbox(XORSTR("AimSpot Debug"), &Settings::Debug::AutoAim::drawTarget);
	ImGui::Checkbox(XORSTR("No Aim Punch"), &Settings::View::NoAimPunch::enabled);
	ImGui::Checkbox(XORSTR("No View Punch"), &Settings::View::NoViewPunch::enabled);
	ImGui::Checkbox(XORSTR("No Scope Border"), &Settings::NoScopeBorder::enabled);
	ImGui::Checkbox(XORSTR("BoneMap Debug"), &Settings::Debug::BoneMap::draw);
	if( Settings::Debug::BoneMap::draw )
		ImGui::Checkbox(XORSTR("Just Dots"), &Settings::Debug::BoneMap::justDrawDots);
}

void Visuals::RenderTab()
{
	// Backend For Visuals
    ImGui::Checkbox(XORSTR("Enabled"), &Settings::ESP::enabled);
	ImGui::SameLine();
	ImGui::PushItemWidth(-1);
    ImGui::Combo( XORSTR( "##BACKENDTYPE" ), (int*)&Settings::ESP::backend, BackendTypes, IM_ARRAYSIZE( BackendTypes ) );
    ImGui::PopItemWidth();

	// Filter Visibility
	ImGui::Columns(2, nullptr, false);
	{
		ImGui::BeginChild(XORSTR("##Visuals"), ImVec2(0, 0), false);
		{
			ImGui::BeginGroupPanel(XORSTR("Enemy"));
			{
				FilterEnemies();
			}ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("LocalPlayer"));
			{
				FilterLocalPlayer();
			}ImGui::EndGroupPanel();

			ImGui::BeginGroupPanel(XORSTR("Alise"));
			{
				FilterAlise();
			}ImGui::EndGroupPanel();


			
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
		ImGui::BeginChild(XORSTR("Chams"), ImVec2(0, 0), false);
		{
			ImGui::Columns(2, nullptr, false);
			{
				ImGui::BeginGroupPanel(XORSTR("Visibility"));
				{
					Visibility();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Player Info"));
				{
					PlayerDetails();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Supportive"));
				{
					Supportive();
				}ImGui::EndGroupPanel();

			}ImGui::NextColumn();
			{
				ImGui::BeginGroupPanel(XORSTR("World Items"));
				{
					Items();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Danger Zone"));
				{
					DangerZone();
				}ImGui::EndGroupPanel();

				ImGui::BeginGroupPanel(XORSTR("Others"));
				{
					Others();
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
					ImGui::Checkbox( XORSTR( "Changes Enabled?" ), &Settings::MaterialConfig::enabled );
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
					ImGui::Checkbox( XORSTR( "GammaTVEnabled" ), &Settings::MaterialConfig::config.m_bGammaTVEnabled );
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

					ImGui::Checkbox( XORSTR( "TripleBuffered" ), &Settings::MaterialConfig::config.m_bTripleBuffered );
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

						ImGui::Checkbox( XORSTR( "Windowed" ), &localFlags[0] );
						ImGui::Checkbox( XORSTR( "Resizing" ), &localFlags[1] );
						ImGui::Checkbox( XORSTR( "No VSYNC Wait" ), &localFlags[3] );
						ImGui::Checkbox( XORSTR( "Stencil" ), &localFlags[4] );
						ImGui::Checkbox( XORSTR( "Force Tri-Linear" ), &localFlags[5] );
						ImGui::Checkbox( XORSTR( "Force HW Sync" ), &localFlags[6] );
						ImGui::Checkbox( XORSTR( "Disable Specular" ), &localFlags[7] );
						ImGui::Checkbox( XORSTR( "Disable Bumpmap" ), &localFlags[8] );
						ImGui::Checkbox( XORSTR( "Disable Phong" ), &localFlags[16] );
						ImGui::Checkbox( XORSTR( "Parallax Mapping" ), &localFlags[9] );
						ImGui::Checkbox( XORSTR( "Use Z-Prefill" ), &localFlags[10] );
						ImGui::Checkbox( XORSTR( "Reduce FillRate" ), &localFlags[11] );
						ImGui::Checkbox( XORSTR( "HDR" ), &localFlags[12] );
						ImGui::Checkbox( XORSTR( "Limit Windowed Size" ), &localFlags[13] );
						ImGui::Checkbox( XORSTR( "Scale to Output Resolution" ), &localFlags[14] );
						ImGui::Checkbox( XORSTR( "Using Multiple Windows" ), &localFlags[15] );
						ImGui::Checkbox( XORSTR( "VR-Mode" ), &localFlags[17] );

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
					ImGui::Checkbox( XORSTR( "EditMode" ), &Settings::MaterialConfig::config.bEditMode );
					//proxiesTestMode
					ImGui::Checkbox( XORSTR ( "Compressed Textures" ),
									 &Settings::MaterialConfig::config.bCompressedTextures );
					ImGui::Checkbox( XORSTR( "Filter Lightmaps" ), &Settings::MaterialConfig::config.bFilterLightmaps );
					ImGui::Checkbox( XORSTR( "Filter Textures" ), &Settings::MaterialConfig::config.bFilterTextures );
					ImGui::Checkbox( XORSTR( "Reverse Depth" ), &Settings::MaterialConfig::config.bReverseDepth );
					ImGui::Checkbox( XORSTR( "Buffer Primitives" ), &Settings::MaterialConfig::config.bBufferPrimitives );
					ImGui::Checkbox( XORSTR( "Draw Flat" ), &Settings::MaterialConfig::config.bDrawFlat );
					ImGui::Checkbox( XORSTR( "Measure Fill-Rate" ), &Settings::MaterialConfig::config.bMeasureFillRate );
					ImGui::Checkbox( XORSTR( "Visualize Fill-Rate" ),
									 &Settings::MaterialConfig::config.bVisualizeFillRate );
					ImGui::Checkbox( XORSTR( "No Transparency" ), &Settings::MaterialConfig::config.bNoTransparency );
					ImGui::Checkbox( XORSTR( "Software Lighting" ),
									 &Settings::MaterialConfig::config.bSoftwareLighting ); // Crashes game
					//AllowCheats ?
					ImGui::SliderInt( XORSTR( "##MIPLEVELS" ), ( int* ) &Settings::MaterialConfig::config.nShowMipLevels, 0,
									  3, XORSTR( "ShowMipLevels: %1.f" ) );
					ImGui::Checkbox( XORSTR( "Show Low-Res Image" ), &Settings::MaterialConfig::config.bShowLowResImage );
					ImGui::Checkbox( XORSTR( "Show Normal Map" ), &Settings::MaterialConfig::config.bShowNormalMap );
					ImGui::Checkbox( XORSTR( "MipMap Textures" ), &Settings::MaterialConfig::config.bMipMapTextures );
					ImGui::SliderInt( XORSTR( "##NFULLBRIGHT" ), ( int* ) &Settings::MaterialConfig::config.nFullbright, 0, 3, XORSTR( "nFullBright: %1.f" ) );
					SetTooltip( XORSTR( "1 = Bright World, 2 = Bright Models" ) );
					ImGui::Checkbox( XORSTR( "Fast NoBump" ), &Settings::MaterialConfig::config.m_bFastNoBump );
					ImGui::Checkbox( XORSTR( "Suppress Rendering" ),
									 &Settings::MaterialConfig::config.m_bSuppressRendering );
					ImGui::Checkbox( XORSTR( "Draw Gray" ), &Settings::MaterialConfig::config.m_bDrawGray );
					ImGui::Checkbox( XORSTR( "Show Specular" ), &Settings::MaterialConfig::config.bShowSpecular );
					ImGui::Checkbox( XORSTR( "Show Defuse" ), &Settings::MaterialConfig::config.bShowDiffuse );
					ImGui::SliderInt( XORSTR( "##AAQUALITY" ), &Settings::MaterialConfig::config.m_nAAQuality, 0, 16,
									  XORSTR( "AAQuality: %1.f" ) );
					ImGui::Checkbox( XORSTR( "Shadow Depth Texture" ),
									 &Settings::MaterialConfig::config.m_bShadowDepthTexture );
					SetTooltip( XORSTR( "Risky. May cause black Screen. Reset if it does." ) );
					ImGui::Checkbox( XORSTR( "Motion Blur" ), &Settings::MaterialConfig::config.m_bMotionBlur );
					ImGui::Checkbox( XORSTR( "Support Flashlight" ),
									 &Settings::MaterialConfig::config.m_bSupportFlashlight );
					ImGui::Checkbox( XORSTR( "Paint Enabled" ), &Settings::MaterialConfig::config.m_bPaintEnabled );
					// VRMode Adapter?
					ImGui::PopItemWidth();
					ImGui::EndPopup();
				}
		}
		ImGui::EndChild();
	}
}
