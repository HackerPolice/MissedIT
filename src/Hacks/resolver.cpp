#include "resolver.h"

#include "../ImGUI/imgui.h"
std::vector<int64_t> Resolver::Players = {};

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
		for (int i = 1; i <= engine->GetMaxClients(); ++i)
		{
			
			C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

			if (!player 
			|| player == localplayer 
			|| player->GetDormant() 
			|| !player->GetAlive() 
			|| player->GetImmune()
			|| Entity::IsTeamMate(player, localplayer))
				continue;

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			// if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
			// 	continue;

			/*
			cvar->ConsoleColorPrintf(ColorRGBA(64, 0, 255, 255), XORSTR("\n[Nimbus] "));
			cvar->ConsoleDPrintf("Debug log here!");
			*/

			// Tanner is a sex bomb, also thank you Stacker for helping us out!
			// float lbyDelta = fabsf(NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y));

			// cvar->ConsoleDPrintf(XORSTR("X Axis : %f\n"), player->GetEyeAngles()->x);
			if (player->GetEyeAngles()->x < 65.f || player->GetEyeAngles()->x > 90.f)
			{
				cvar->ConsoleDPrintf(XORSTR("Resolving : Legit AA"));
				cvar->ConsoleDPrintf(XORSTR("MissedShots : %d\n"), players[player->GetIndex()].MissedCount);
				static float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);

				switch(Resolver::players[player->GetIndex()].MissedCount)
				{
					case 0:
						player->GetAnimState()->goalFeetYaw = trueDelta + player->GetEyeAngles()->y;
						break;
					case 1:
						break;
					case 2:
						player->GetAnimState()->goalFeetYaw = trueDelta ? player->GetEyeAngles()->y + trueDelta :  player->GetEyeAngles()->y - 57.f;
						break;
					case 3:
						player->GetAnimState()->goalFeetYaw = trueDelta ? player->GetEyeAngles()->y + trueDelta : player->GetEyeAngles()->y + 57.f;
						break;
					case 4:
						player->GetEyeAngles()->y = trueDelta ? player->GetEyeAngles()->y + trueDelta :  player->GetEyeAngles()->y - 57.f;
						break;
					case 5:
						player->GetEyeAngles()->y += trueDelta;
						break;
					case 6:
						player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (trueDelta/2);
						break;
					default:
						break;
				}
														
			}
			else
            {
                float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
				cvar->ConsoleDPrintf(XORSTR("Resolving : Rage AA"));
				cvar->ConsoleDPrintf(XORSTR("MissedShots : %d\n"), players[player->GetIndex()].MissedCount);
				switch(Resolver::players[player->GetIndex()].MissedCount)
				{
					case 0:
						player->GetEyeAngles()->y +=  trueDelta;
						break;
					case 1:
						break;
					case 2:
						player->GetEyeAngles()->y = trueDelta ? player->GetEyeAngles()->y + trueDelta :  player->GetEyeAngles()->y - 57.f;
						break;
					case 3:
						player->GetEyeAngles()->y = trueDelta ? player->GetEyeAngles()->y - 57.f : player->GetEyeAngles()->y + trueDelta;
						break;
					case 4:
						player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + trueDelta;
						break;
					case 5:
						player->GetEyeAngles()->y +=  (trueDelta/2);
						break;
					case 6:
						player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (trueDelta/2);
						break;
					default:
						break;
				}
				
            }	
		}
	}
	else if (stage == ClientFrameStage_t::FRAME_RENDER_END)
	{
		// for (unsigned long i = 0; i < player_data.size(); i++)
		// {
		// 	std::pair<C_BasePlayer *, QAngle> player_aa_data = player_data[i];
		// 	*player_aa_data.first->GetEyeAngles() = player_aa_data.second;
		// }

		// player_data.clear();
	}
}

void Resolver::FireGameEvent(IGameEvent *event)
{	

	if (!event)
		return;
	// cvar->ConsoleDPrintf(XORSTR("\nPlayer Hurt : %d\n"), strcmp(event->GetName(), "player_hurt"));
	if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 || strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0)
    {
		if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
	    	return;
    }

		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) // suicide
	    	return;
		
		if (attacker_id != engine->GetLocalPlayer())
			return;

		if (strcmp(event->GetName(), "player_hurt") == 0 || strcmp(event->GetName(), "player_hurt") == -1);		
			// Resolver::players[TargetID].MissedCount--;
				// ImGui::TextWrapped(XORSTR("Missed"));
}
