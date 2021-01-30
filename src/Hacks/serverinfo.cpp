#include "serverinfo.h"
#include "HeaderFiles.h"
#include "acsafe.h"

void ServerInfo::FireGameEvent(IGameEvent *event)
{
	if (!event) {
		return;
	}

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) != 0) {
		return;
	}

	if (event->GetInt(XORSTR("userid")) &&
	    engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer()) {
		return;
	}

	gameMode = cvar->FindVar("game_mode")->GetInt();
	isValveDS = (*csGameRules)->IsValveDS();

	ACSafe::AntiVACKick = isValveDS && gameMode != 1 && gameMode != 2;
}
