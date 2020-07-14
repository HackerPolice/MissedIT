#include "resolver.h"

#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "../interfaces.h"
#include "../settings.h"
#include "antiaim.h"

std::vector<int64_t> Resolver::Players = {};
std::vector<std::pair<C_BasePlayer *, QAngle>> player_data_Nimbus;

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

void Resolver::CreateMove(CUserCmd* cmd)
{
	if (!engine->IsInGame() || !Settings::Resolver::resolveAll)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	static const int &maxClient = engine->GetMaxClients();

	for (int i = maxClient; i > 0; i--)
	{
		C_BasePlayer *player = (C_BasePlayer *)entityList->GetClientEntity(i);

		if (!player 
			|| player == localplayer 
			|| player->GetDormant() 
			|| !player->GetAlive() 
			|| player->GetImmune() 
			|| Entity::IsTeamMate(player, localplayer))
			{ continue; }

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
				continue;

			player_data_Nimbus.push_back(std::pair<C_BasePlayer *, QAngle>(player, *player->GetEyeAngles()));

			// Tanner is a sex bomb, also thank you Stacker for helping us out!
			// float lbyDelta = fabsf(NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y));

			// if (lbyDelta < 35)
			// 	return;
			// if (player->GetFlags() & IN_ATTACK)	
			// 	continue;

			
			float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
			if (player->GetFlags() & FL_DUCKING)
			{
				float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);

				if (player->GetVelocity().Length() < 10.0f)
				{
					if (trueDelta < 0)
					{
						// player->GetAnimState()->goalFeetYaw = 
						*player->GetLowerBodyYawTarget() = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.1) );
					}
					else if (trueDelta > 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
						*player->GetLowerBodyYawTarget() = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.1) ) - player->GetEyeAngles()->y;
					}
				}	
				else
				{
					float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
					
					if (trueDelta <= 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
							*player->GetLowerBodyYawTarget() = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())));
					}
					else if (trueDelta > 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
							*player->GetLowerBodyYawTarget() = fabs(-AntiAim::GetMaxDelta(player->GetAnimState())) - player->GetEyeAngles()->y;
					}
				}
			}			
			
			else if (player->GetVelocity().Length2D() >= 10.0f && player->GetVelocity().Length2D() < 50.f) // fake walking
			{
				if (trueDelta < 0)
				{
						// player->GetAnimState()->goalFeetYaw;
						*player->GetLowerBodyYawTarget() = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.7));
						// player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.7));
						// player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.7));
						// player->GetAnimState()->goalFeetYaw = 
						// player->GetEyeAngles()->y = prevYW + static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else if (trueDelta > 0)
				{
					
						// player->GetAnimState()->goalFeetYaw;
						*player->GetLowerBodyYawTarget() = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.7) - player->GetEyeAngles()->y;
						// player->GetAnimState()->goalFeetYaw = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.5) - player->GetEyeAngles()->y;
						// player->GetEyeAngles()->y = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.99f) - player->GetEyeAngles()->y;
						// player->GetAnimState()->goalFeetYaw = 
						// player->GetEyeAngles()->y = prevYW - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else 
				{
					static bool bFlip = false;
					bFlip = bFlip;
					if (bFlip)
						// player->GetAnimState()->goalFeetYaw = 
						*player->GetLowerBodyYawTarget() = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) - player->GetEyeAngles()->y;
						// player->GetEyeAngles()->y = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) - player->GetEyeAngles()->y;
					else
						*player->GetLowerBodyYawTarget() = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) + player->GetEyeAngles()->y;
						// player->GetEyeAngles()->y  = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) + player->GetEyeAngles()->y;
				}
			}
			
			else
			{
				float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
					
				if (trueDelta < 0)
				{
					
					// player->GetAnimState()->goalFeetYaw;
					*player->GetLowerBodyYawTarget() = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.4));
					// player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.3));
					// player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.4));
					// player->GetAnimState()->goalFeetYaw = 
					// player->GetEyeAngles()->y = prevYW + static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else if (trueDelta > 0)
				{
					
					// player->GetAnimState()->goalFeetYaw;
					// player->GetAnimState()->goalFeetYaw = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.3f) - player->GetEyeAngles()->y;
					*player->GetLowerBodyYawTarget() = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.4) - player->GetEyeAngles()->y;
					// player->GetAnimState()->goalFeetYaw = 
					// player->GetEyeAngles()->y = prevYW = static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else 
				{
					static bool bFlip = false;
					bFlip = bFlip;
					if (bFlip)
						// player->GetAnimState()->goalFeetYaw = 
						*player->GetLowerBodyYawTarget() = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) - player->GetEyeAngles()->y;
						// player->GetEyeAngles()->y = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) - player->GetEyeAngles()->y;
					else
						*player->GetLowerBodyYawTarget() = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) + player->GetEyeAngles()->y;
						// player->GetEyeAngles()->y  = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5) ) + player->GetEyeAngles()->y;
				}
			}
	}
}

void Resolver::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!engine->IsInGame() || !Settings::Resolver::resolveAll)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = entityList->GetHighestEntityIndex(); i > 1; i--)
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

			if (!Settings::Resolver::resolveAll && std::find(Resolver::Players.begin(), Resolver::Players.end(), entityInformation.xuid) == Resolver::Players.end())
				continue;

			player_data_Nimbus.push_back(std::pair<C_BasePlayer *, QAngle>(player, *player->GetEyeAngles()));

			// Tanner is a sex bomb, also thank you Stacker for helping us out!
			float lbyDelta = fabsf(NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y));

			if (lbyDelta < 35)
				return;
			if (player->GetFlags() & IN_ATTACK)	
				continue;

			
			float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
			if (player->GetFlags() & FL_DUCKING)
			{
				float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);

				if (player->GetVelocity().Length() < 10.0f)
				{
					if (trueDelta < 0)
					{
						// player->GetAnimState()->goalFeetYaw = 
						player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.2) );
					}
					else if (trueDelta > 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
						player->GetEyeAngles()->y = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.2) ) - player->GetEyeAngles()->y;
					}
				}	
				else
				{
					float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
					
					if (trueDelta <= 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
							player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.6));
					}
					else if (trueDelta > 0)
					{
							// player->GetAnimState()->goalFeetYaw = 
							player->GetEyeAngles()->y = fabs(-AntiAim::GetMaxDelta(player->GetAnimState())*0.6) - player->GetEyeAngles()->y;
					}
				}
			}			
			
			else if (player->GetVelocity().Length() < 10.0f)
			{
				if (trueDelta < 0)
				{
					
						// player->GetAnimState()->goalFeetYaw;
					// player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.7));
					player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.5));
						// player->GetAnimState()->goalFeetYaw = 
						// player->GetEyeAngles()->y = prevYW + static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else if (trueDelta > 0)
				{
					
						// player->GetAnimState()->goalFeetYaw;
						player->GetAnimState()->goalFeetYaw = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.5) - player->GetEyeAngles()->y;
					// player->GetEyeAngles()->y = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.99f) - player->GetEyeAngles()->y;
						// player->GetAnimState()->goalFeetYaw = 
						// player->GetEyeAngles()->y = prevYW - static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else 
				{
					static bool bFlip = false;
					bFlip = bFlip;
					if (bFlip)
						// player->GetAnimState()->goalFeetYaw = 
						player->GetEyeAngles()->y = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.2) ) - player->GetEyeAngles()->y;
					else
						player->GetEyeAngles()->y  = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.2) ) + player->GetEyeAngles()->y;
				}
			}
			
			else
			{
				float trueDelta = NormalizeAsYaw(*player->GetLowerBodyYawTarget() - player->GetEyeAngles()->y);
					
				if (trueDelta < 0)
				{
					
					// player->GetAnimState()->goalFeetYaw;
					// player->GetAnimState()->goalFeetYaw = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.3));
					player->GetEyeAngles()->y = player->GetEyeAngles()->y + (fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.4));
					// player->GetAnimState()->goalFeetYaw = 
					// player->GetEyeAngles()->y = prevYW + static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else if (trueDelta > 0)
				{
					
					// player->GetAnimState()->goalFeetYaw;
					// player->GetAnimState()->goalFeetYaw = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.3f) - player->GetEyeAngles()->y;
					player->GetEyeAngles()->y = fabs(-AntiAim::GetMaxDelta(player->GetAnimState()) * 0.4f) - player->GetEyeAngles()->y;
					// player->GetAnimState()->goalFeetYaw = 
					// player->GetEyeAngles()->y = prevYW = static_cast<float>(static_cast<float>(std::rand())/static_cast<float>(RAND_MAX/fabs(prevYW-player->GetAnimState()->goalFeetYaw)));
				}
				else 
				{
					static bool bFlip = false;
					bFlip = bFlip;
					if (bFlip)
						player->GetAnimState()->goalFeetYaw = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.3) ) - player->GetEyeAngles()->y;
					else
						player->GetAnimState()->goalFeetYaw = ( fabs(AntiAim::GetMaxDelta(player->GetAnimState())*0.3) ) - player->GetEyeAngles()->y;
				}
			}
		}
	}
	else if (stage == ClientFrameStage_t::FRAME_RENDER_END)
	{
		for (unsigned long i = 0; i < player_data_Nimbus.size(); i++)
		{
			std::pair<C_BasePlayer *, QAngle> player_aa_data = player_data_Nimbus[i];
			*player_aa_data.first->GetEyeAngles() = player_aa_data.second;
		}

		player_data_Nimbus.clear();
	}
}

void Resolver::FireGameEvent(IGameEvent *event)
{
	if (!engine->IsInGame() || !Settings::Resolver::resolveAll)
		return;

	C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->GetAlive())
		return;

	if (!event)
		return;

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0 && strcmp(event->GetName(), XORSTR("cs_game_disconnected")) != 0)
		return;

	if (event->GetInt(XORSTR("userid")) && engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer())
		return;

	player_data_Nimbus.clear();
}
