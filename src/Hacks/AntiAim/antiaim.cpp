#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wmisleading-indentation"

#include "antiaim.h"

#include "../AimBot/ragebot.hpp"

#include "../AimBot/aimbot.hpp"
#include "fakeduck.h"
#include "fakewalk.hpp"
#include "slowwalk.hpp"
#include "../acsafe.h"
#include "../aimstep.h"

#define GetPercentVal(val, percent) (val * (percent/100.f))

float AntiAim::GetMaxDelta(CCSGOAnimState *animState)
{
	float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

	float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

	float unk1 = ((animState->runningAccelProgress * -0.30000001) - 0.19999999) * speedFraction;
	unk1 = unk1 + 1.0f;
	float delta;

	if (animState->duckProgress > 0) {
		unk1 += ((animState->duckProgress * speedFactor) * (0.5f - unk1));// - 1.f
	}

	delta = *(float *) ((uintptr_t) animState + 0x3A4) * unk1;

	if (ACSafe::AntiVACKick) {
		return (delta - 0.5f) / 2.0f;
	}

	return delta - 0.5f;
}

static C_BasePlayer *GetClosestEnemy(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return nullptr;
	}
	C_BasePlayer *closestPlayer = nullptr;
	Vector pVecTarget = localplayer->GetEyePosition();
	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);
	float prevFOV = 0.f;

	int maxPlayers = engine->GetMaxClients();
	for (int i = 1; i < maxPlayers; ++i) {
		C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player
		    || player == localplayer
		    || player->GetDormant()
		    || !player->IsAlive()
		    || player->GetImmune()) {
			continue;
		}

		if (Entity::IsTeamMate(player, localplayer)) {
			continue;
		}

		Vector cbVecTarget = player->GetAbsOrigin();

		float cbFov = Math::GetFov(viewAngles, Math::CalcAngle(pVecTarget, cbVecTarget));

		if (prevFOV == 0.f) {
			prevFOV = cbFov;
			closestPlayer = player;
		} else if (cbFov < prevFOV) {
			prevFOV = cbFov;
			closestPlayer = player;
		}
	}
	return closestPlayer;
}

/*
static float GetBestHeadAngle(CUserCmd* cmd)
{    
    C_BasePlayer *localplayer = (C_BasePlayer *)entityList->GetClientEntity(engine->GetLocalPlayer());
	
    if (!localplayer || !localplayer->IsAlive())
		return 0;

    // C_BasePlayer* target = GetClosestEnemy(cmd);
    // we will require these eye positions
	Vector eye_position = localplayer->GetEyePosition();
	// lets set up some basic values we need
	int best_damage = localplayer->GetHealth();
	// this will result in a 45.0f deg step, modify if you want it to be more 'precise'
    static const float angle_step = 45.f;
	// our result
	float yaw = 0.0f;
    for (int i = 1; i <= engine->GetMaxClients() ; i++)
	{
        C_BasePlayer* player = (C_BasePlayer*) entityList->GetClientEntity(i);

		if (!player || 
			player == localplayer || 
			player->GetDormant() || 
			!player->IsAlive() || 
			player->GetImmune())
			continue;

        if (Entity::IsTeamMate(player, localplayer))
            return 0;
        // iterate through 45.0f deg angles  
	    for( float n = 0.0f; n < 180.f; n += angle_step )
	    {
	    	// shoutout to aimtux for headpos calc
	        Vector head_position( cos( n ) + eye_position.x,
						            sin( n ) + eye_position.y,
						            eye_position.z);

            AutoWall::FireBulletData data;
            int damage = AutoWall::GetDamage(player, head_position, true, data);
            // cvar->ConsoleDPrintf(XORSTR("AUTO DIRECTION DAMAGE : %d \n"), damage);
		    if( damage < best_damage && damage > -1)
		    {
		        best_damage = damage;
	            yaw = n;
		    }
	    }
    }
    return yaw;
	
}
*/

static bool GetBestHeadAngle(CUserCmd *cmd, QAngle &angle)
{
	if (!Settings::AntiAim::autoDirection) {
		return false;
	}

	float b, r, l;

	Vector src3D, dst3D, forward, right, up, src, dst;

	trace_t tr;
	Ray_t ray, ray2, ray3, ray4, ray5;
	CTraceFilter filter;

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return false;
	}

	QAngle viewAngles;
	engine->GetViewAngles(viewAngles);

	viewAngles.x = 0;

	Math::AngleVectors(viewAngles, &forward, &right, &up);

	auto GetTargetEntity = [&](void) {
		C_BasePlayer *bestTarget = nullptr;

		float prevFov = 0;
		const int &MaxClient = engine->GetMaxClients();
		for (int i = 1; i < MaxClient; i++) {
			C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);

			if (!player
			    || player == localplayer
			    || player->GetDormant()
			    || !player->IsAlive()
			    || player->GetImmune()
			    || player->GetTeam() == localplayer->GetTeam()) {
				continue;
			}

			float fov = Math::GetFov(viewAngles,
			                         Math::CalcAngle(localplayer->GetEyePosition(), player->GetEyePosition()));

			if (prevFov == 0) {
				bestTarget = player;
				prevFov = fov;
			} else if (fov > prevFov) {
				bestTarget = player;
				prevFov = fov;
			}
		}

		return bestTarget;
	};

	C_BasePlayer *target = GetTargetEntity();
	if (!target) {
		return false;
	}
	// cvar->ConsoleDPrintf("Found Target\n");
	filter.pSkip = localplayer;
	src3D = localplayer->GetEyePosition();
	dst3D = src3D + (forward * 384);
	dst3D = target->GetBonePosition(BONE_HEAD);

	if (target == nullptr) {
		return false;
	}

	ray.Init(src3D, dst3D);
	trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	b = (tr.endpos - tr.startpos).Length();

	ray2.Init(src3D + right * 35, dst3D + right * 35);
	trace->TraceRay(ray2, MASK_SHOT, &filter, &tr);
	r = (tr.endpos - tr.startpos).Length();

	ray3.Init(src3D - right * 35, dst3D - right * 35);
	trace->TraceRay(ray3, MASK_SHOT, &filter, &tr);
	l = (tr.endpos - tr.startpos).Length();

	if (b < r && b < l && l == r) {
		return false;
	} //if left and right are equal and better than back

	if (b > r && b > l) {
		AntiAim::realAngle.y = angle.y -= 180; //if back is the best angle
	} else if (r > l && r > b) {
		AntiAim::realAngle.y = angle.y += 90; //if right is the best angle
	} else if (r > l && r == b) {
		AntiAim::realAngle.y = angle.y += 135; //if right is equal to back
	} else if (l > r && l > b) {
		AntiAim::realAngle.y = angle.y -= 90; //if left is the best angle
	} else if (l > r && l == b) {
		AntiAim::realAngle.y = angle.y -= 135; //if left is equal to back
	} else {
		return false;
	}

	return true;
}

static bool LBYBreak(C_BasePlayer *localplayer, bool notsendBreak)
{
	static bool lbyBreak;
	static float lastCheck = 0.f;

	if (!lbyBreak && (globalVars->curtime - lastCheck) > 0.22) {
		lbyBreak = true;
		lastCheck = globalVars->curtime;
		if (notsendBreak) { CreateMove::sendPacket = AntiAim::bSend = false; }
	} else if (lbyBreak && (globalVars->curtime - lastCheck) > 1.1) {
		lbyBreak = false;
		lastCheck = globalVars->curtime;
	} else {
		lbyBreak = false;
	}

	return lbyBreak;
}

static bool LbyPrediction(CCSGOAnimState *animState)
{
	static float next_lby_update = 0;
	if (!animState) {
		return false;
	}

	if (animState->feetShuffleSpeed > 0.1f) {
		next_lby_update = globalVars->curtime + 0.22f;
	} else if (globalVars->curtime > next_lby_update) {
		next_lby_update = globalVars->curtime + 1.1f;
		return true;
	}

	return false;
}

static void DoManualAntiAim()
{
	static bool B_pressed = false;

	if (!Settings::AntiAim::ManualAntiAim::Enable) {
		return;
	}

	if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton) && !B_pressed) {
		AntiAim::ManualAntiAim::alignBack = !AntiAim::ManualAntiAim::alignBack;
		AntiAim::ManualAntiAim::alignLeft = AntiAim::ManualAntiAim::alignRight = false;
		B_pressed = true;
	} else if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) && !B_pressed) {
		AntiAim::ManualAntiAim::alignRight = !AntiAim::ManualAntiAim::alignRight;
		AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignLeft = false;
		B_pressed = true;
	} else if (inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) && !B_pressed) {
		AntiAim::ManualAntiAim::alignBack = AntiAim::ManualAntiAim::alignRight = false;
		AntiAim::ManualAntiAim::alignLeft = !AntiAim::ManualAntiAim::alignLeft;
		B_pressed = true;
	}
	bool buttonNotPressed = !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::LeftButton) &&
	                        !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::RightButton) &&
	                        !inputSystem->IsButtonDown(Settings::AntiAim::ManualAntiAim::backButton);

	if (buttonNotPressed && B_pressed) {
		B_pressed = false;
	}
}

static bool canMove(C_BasePlayer *localplayer, C_BaseCombatWeapon *activeweapon, CUserCmd *cmd)
{
	if (AimStep::inProgress) {
		return false;
	}

	if (activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_GRENADE) {
		C_BaseCSGrenade *csGrenade = (C_BaseCSGrenade *) activeweapon;
		if (csGrenade->GetThrowTime() > 0.f) {
			return false;
		}
	}

	if (cmd->buttons & IN_ATTACK) {
		return false;
	}
	if (cmd->buttons & IN_USE) {
		return false;
	}
	if (cmd->buttons & IN_ATTACK2 && activeweapon->GetCSWpnData()->GetWeaponType() == CSWeaponType::WEAPONTYPE_KNIFE) {
		return false;
	}

	if (localplayer->GetMoveType() == MOVETYPE_LADDER || localplayer->GetMoveType() == MOVETYPE_NOCLIP) {
		return false;
	}

	return true;
}

static void DoAntiAim(CUserCmd *cmd, C_BasePlayer *localplayer, QAngle &angle)
{

	if (GetBestHeadAngle(cmd, angle)) {
		return;
	}

	float maxDelta = AntiAim::GetMaxDelta(localplayer->GetAnimState());

	if (Settings::AntiAim::dsynctype == DsyncType::type1) {
		if (!AntiAim::bSend) {
			if (LbyPrediction(localplayer->GetAnimState())) {
				Settings::AntiAim::inverted ? angle.y -= maxDelta * 2 : angle.y += maxDelta * 2;
			} else {
				Settings::AntiAim::inverted ? angle.y += maxDelta * 2 : angle.y -= maxDelta * 2;
			}
		}
	} else {
		if (!(cmd->buttons & IN_FORWARD || cmd->buttons & IN_BACK || cmd->buttons & IN_MOVELEFT ||
		      cmd->buttons & IN_MOVERIGHT) && localplayer->GetFlags() & FL_ONGROUND) {
			cmd->forwardmove = cmd->tick_count % 2 ? localplayer->GetFlags() & IN_DUCK ? 2.94117647f : .505f :
			                   localplayer->GetFlags() & IN_DUCK ? -2.94117647f : -.505f;
		}
		if (!AntiAim::bSend) {
			Settings::AntiAim::inverted ? angle.y += maxDelta * 2 : angle.y -= maxDelta * 2;
		}
	}
	// if (LBYBreak(localplayer, true)){
	//      if (Settings::AntiAim::inverted) {
	//         AntiAim::fakeAngle.y = AntiAim::realAngle.y = 0;
	//         AntiAim::fakeAngle.x = AntiAim::realAngle.x = angle.x;
	//         angle.y += maxDelta;
	//     }
	//     else {
	//         AntiAim::fakeAngle.y = AntiAim::realAngle.y = 0;
	//         AntiAim::realAngle.x = angle.x;
	//         angle.y -= maxDelta;
	//     }
	// }
	// else if ( !AntiAim::bSend ){

	//     AntiAim::realAngle.y = angle.y-28.f;
	//     AntiAim::realAngle.x = angle.x;
	//     Settings::AntiAim::inverted ?  angle.y -= maxDelta*2 : angle.y += maxDelta*2;
	// }
	// else {
	//     if (Settings::AntiAim::inverted) {

	//         AntiAim::fakeAngle.y = angle.y-28.f;
	//         AntiAim::fakeAngle.x = angle.x;
	//     }
	//     else {
	//         AntiAim::fakeAngle.y = angle.y+28.f;
	//         AntiAim::fakeAngle.x = angle.x;
	//     }
	// }

}

void AntiAim::CreateMove(CUserCmd *cmd)
{
	if (!Settings::AntiAim::Enabled) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}
	C_BaseCombatWeapon *activeWeapon = (C_BaseCombatWeapon *) entityList->GetClientEntityFromHandle(
			localplayer->GetActiveWeapon());

	if (!activeWeapon) { return; }

	if (!canMove(localplayer, activeWeapon, cmd)) {
		CreateMove::lastTickViewAngles = AntiAim::realAngle = AntiAim::fakeAngle = cmd->viewangles;
		return;
	}

	if (Settings::AntiAim::FakeDuck::enabled && FakeDuck::FakeDucking) {
		AntiAim::bSend = CreateMove::sendPacket;
	} else if (Settings::AntiAim::SlowWalk::enabled && SlowWalk::SlowWalking) {
		AntiAim::bSend = CreateMove::sendPacket;
	} else if (Settings::AntiAim::FakeWalk::enabled && FakeWalk::FakeWalking) {
		AntiAim::bSend = CreateMove::sendPacket;
	}
	if (Settings::FakeLag::enabled) {
		AntiAim::bSend = CreateMove::sendPacket;
	} else {
		CreateMove::sendPacket = AntiAim::bSend = !AntiAim::bSend;
	}

	QAngle angle = cmd->viewangles;
	QAngle oldAngle;
	engine->GetViewAngles(oldAngle);

	if (Settings::AntiAim::atTheTarget) {
		C_BasePlayer *lockedTarget = GetClosestEnemy(cmd);
		if (lockedTarget) {
			Math::CalcAngle(localplayer->GetEyePosition(), lockedTarget->GetEyePosition(), angle);
		}
	}
	if (Settings::AntiAim::PitchDown) {
		angle.x = 89.f;
	}

	DoManualAntiAim();
	if (AntiAim::ManualAntiAim::alignBack) {
		angle.y += 180;
		AntiAim::realAngle = AntiAim::fakeAngle = angle;
	} else if (AntiAim::ManualAntiAim::alignRight) {
		angle.y -= 90;
		AntiAim::realAngle = AntiAim::fakeAngle = angle;
	} else if (AntiAim::ManualAntiAim::alignLeft) {
		angle.y += 90;
		AntiAim::realAngle = AntiAim::fakeAngle = angle;
	} else if (Settings::AntiAim::Jitter::Value > 0) {

		angle.y += Settings::AntiAim::offset;
		static bool flip = false;
		// swap angle
		angle.y += flip ? Settings::AntiAim::Jitter::Value : -Settings::AntiAim::Jitter::Value;

		if (Settings::AntiAim::Jitter::SyncWithLag) {
			CreateMove::sendPacket ? flip = !flip : flip;
		} else {
			flip = !flip;
		}

	} else {
		angle.y += Settings::AntiAim::offset;

		static bool B_pressed = false;
		if (inputSystem->IsButtonDown(Settings::AntiAim::InvertKey) && !B_pressed) {
			Settings::AntiAim::inverted = !Settings::AntiAim::inverted;
			B_pressed = true;
		} else if (!inputSystem->IsButtonDown(Settings::AntiAim::InvertKey) && B_pressed) {
			B_pressed = false;
		}

		if (Settings::AntiAim::lbyBreak::Enabled) {
			if (LBYBreak(localplayer, Settings::AntiAim::lbyBreak::notSend)) {
				angle.y += Settings::AntiAim::inverted ? Settings::AntiAim::lbyBreak::angle :
				           Settings::AntiAim::lbyBreak::angle * -1;
			}
		} else if (Settings::AntiAim::EnableFakAngle) {
			DoAntiAim(cmd, localplayer, angle);
		}
	}

	if (CreateMove::sendPacket && Settings::AntiAim::JitterFake) {
		Settings::AntiAim::inverted = !Settings::AntiAim::inverted;
	}

	Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

	CreateMove::lastTickViewAngles = angle;

	cmd->viewangles = angle;

	Aimbot::FixMouseDeltas(cmd, angle, oldAngle);
	Math::CorrectMovement(oldAngle, cmd, cmd->forwardmove, cmd->sidemove);
}

void AntiAim::FrameStageNotify(ClientFrameStage_t stage)
{

}

void AntiAim::OverrideView(CViewSetup *pSetup)
{

}
