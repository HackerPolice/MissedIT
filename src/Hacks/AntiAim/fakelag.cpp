#include "fakelag.h"
#include "../AimBot/autowall.h"

#ifndef absol
#define absol(x) x < 0 ? x*-1 : x
#endif

void FakeLag::CreateMove(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	} 
	
	C_BasePlayer *player = nullptr;
	int maxClient = engine->GetMaxClients();
	int damage = 0;
	for (int i = 1; i < maxClient; ++i) {
		player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player ||
		    i == engine->GetLocalPlayer() ||
		    player->GetDormant() ||
		    !player->IsAlive() ||
		    player->GetImmune() ||
		    player->GetTeam() == localplayer->GetTeam()) {
			continue;
		}

		AutoWall::FireBulletData data;
		int _damage = AutoWall::GetDamage(player, localplayer->GetEyePosition(), true, data);
		if ( _damage > damage )
			damage = _damage;

	}

	//  basically to ensure that if a player pussing or you are pushing a player fakelag with benefit you
	// Though I don't know if it shoudl work or not let's try
	if ( damage > 0 ){ 
		FakeLag::ticks = 1;
	}
	else if (FakeLag::ticks >= Settings::FakeLag::value) {
		CreateMove::sendPacket = true;
		FakeLag::ticks = Settings::FakeLag::impulseLag ? -1 * Settings::FakeLag::value : -1;
	} else if (FakeLag::ticks > 0) {
		CreateMove::sendPacket = false;
	}

	FakeLag::ticks++;

}
