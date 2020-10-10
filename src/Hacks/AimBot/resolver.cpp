#include "resolver.h"

#include "../ImGUI/imgui.h"
std::vector<int64_t> Resolver::Players = {};

#define RANDOME_FLOAT(x) ( static_cast<float>(static_cast<float>(rand()/ static_cast<float>(RAND_MAX/ ( x ) ))) )
#define GetPercentVal(val, percent) (val * (percent/100.f))

static float NormalizeAsYaw(float flAngle)
{
	if (flAngle > 180.f || flAngle < -180.f)
	{
		auto revolutions = round(abs(flAngle / 360.f));

		if (flAngle < 0.f)
			flAngle += 360.f * revolutions;
		else
			flAngle -= 360.f * revolutions;
	}

	return flAngle;
}

void Resolver::AnimationFix(C_BasePlayer *player)
{
	// player->ClientAnimations(true);

	// auto old_curtime = globalVars->curtime;
	// auto old_frametime = globalVars->frametime;

	// globalVars->curtime = player->GetSimulationTime();
	// globalVars->frametime = globalVars->interval_per_tick;

	// auto player_animation_state = player->GetAnimState();
	// auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
	// if (player_animation_state && player_model_time)
	// 	if (*player_model_time == globalVars->frametime)
	// 		* player_model_time = globalVars->frametime - 1;


	// player->updateClientAnimation();

	// globalVars->curtime = old_curtime;
	// globalVars->frametime = old_frametime;

	//pEnt->SetAbsAngles(Vector3(0, player_animation_state->m_flGoalFeetYaw, 0));

	// player->ClientAnimations(false);
}

void Resolver::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!engine->IsInGame())
		return;

	if (!Settings::Resolver::resolveAll)
		return;
		
	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		
		int maxClient = engine->GetMaxClients();
		for (int i = 1; i < maxClient; i++)
		{
			C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);
			// Resolver::AnimationFix(player);

			if (!player 
			|| player == localplayer 
			|| player->GetDormant() 
			|| !player->GetAlive() 
			|| player->GetImmune()
			|| Entity::IsTeamMate(player, localplayer))
				continue;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if ( Resolver::players[player->GetIndex()].enemy )
			{
				if (player != Resolver::players[player->GetIndex()].enemy) // It means player discoennected or player sequence changed better to reset out miss shots count
				{
					Resolver::players[player->GetIndex()].MissedCount = 0;
					Resolver::players[player->GetIndex()].enemy = player;
				}
			}
			else 
			{
				Resolver::players[player->GetIndex()].enemy = player;
			}

			// player->GetEyeAngles()->y += 60;
            float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);

			// 
			switch(Resolver::players[player->GetIndex()].MissedCount)
			{
				case 0:
					player->GetEyeAngles()->y += trueDelta; 
					Resolver::players[player->GetIndex()].PrevTrueDelta = trueDelta;
					break;
				case 1:
					break;
				case 2:
					if (Resolver::players[player->GetIndex()].PrevTrueDelta == trueDelta)
						player->GetEyeAngles()->y -= trueDelta;
					break;
				case 3:
					Resolver::players[player->GetIndex()].MissedCount = 0;
					player->GetEyeAngles()->y += trueDelta; 
					break;
				default:
					break;
			}
        }	
	}
}

void Resolver::FireGameEvent(IGameEvent *event)
{	
	// no need right now
}
