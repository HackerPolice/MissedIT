#pragma once

#include "../HeaderFiles.h"
#include "../AntiAim/antiaim.h"
#include "ragebot.hpp"

namespace Resolver
{

	struct resolvedPlayers
	{
		C_BasePlayer *enemy;
		int PrevTrueDelta = 0;
		int MissedCount = 0;
	};

	inline resolvedPlayers players[1000];
	inline std::pair<C_BasePlayer *, int> player;
	extern std::vector<int64_t> Players;

	//Hooks
	void FrameStageNotify(ClientFrameStage_t stage); // This is where the resolver happens.
	void FireGameEvent(IGameEvent *event);             // Strangely, I never see this getting called.
	void CreateMove(CUserCmd *cmd);

	void AnimationFix(C_BasePlayer *player);

} 
