#include "resolver.h"

std::vector<int64_t> Resolver::Players = {};

#define RANDOME_FLOAT(x) ( static_cast<float>(static_cast<float>(rand()/ static_cast<float>(RAND_MAX/ ( x ) ))) )
#define GetPercentVal(val, percent) (val * (percent/100.f))

static float NormalizeAsYaw(float flAngle)
{
	if (flAngle > 180.f || flAngle < -180.f) {
		auto revolutions = round(abs(flAngle / 360.f));

		if (flAngle < 0.f) {
			flAngle += 360.f * revolutions;
		} else {
			flAngle -= 360.f * revolutions;
		}
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

float NormalizeYaw(float yaw)
{
	if (yaw > 180) {
		yaw -= (round(yaw / 360) * 360.f);
	}

	if (yaw < -180) {
		yaw += (round(yaw / 360) * -360.f);
	}

	return yaw;
}

float flAngleMod(float flAngle)
{
	return ((360.0f / 65536.0f) * (int32_t(flAngle * (65536.0f / 360.0f)) & 65535));
}

float ApproachAngle(float target, float value, float speed)
{
	target = flAngleMod(target);
	value = flAngleMod(value);

	float delta = target - value;
	if (speed < 0) {
		speed = -speed;
	}

	if (delta < -180) {
		delta += 360;
	} else if (delta > 180) {
		delta -= 360;
	}

	if (delta > speed) {
		value += speed;
	} else if (delta < -speed) {
		value -= speed;
	} else {
		value = target;
	}

	return value;
}

float yawDistance(float first, float second)
{
	if (first == second) {
		return 0.f;
	}

	if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f)) {
		return std::fabs(first - second);
	} //where the fuck is fabsf?

	if (first > 90.f && second < -90.f) {
		first -= (first - 90.f);
		second += (second + 90.f);
	} else if (first < -90.f && second > 90.f) {
		first += (first + 90.f);
		second -= (second - 90.f);
	}

	return std::fabs(first - second);
}

float resLBY(float first, float second)
{
	if (first == second) {
		return 0.f;
	}

	if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f)) {
		return std::fabs(first - second);
	} //where the fuck is fabsf?

	if (first > 35.f && second < -35.f) {
		first -= (first - 35.f);
		second += (second + 35.f);
	} else if (first < -35.f && second > 35.f) {
		first += (first + 35.f);
		second -= (second - 35.f);
	}

	return std::fabs(first - second);
}

float GetMaxDeltaAP(CCSGOAnimState *animState)
{

	float speedFraction = std::max(0.0f, std::min(animState->feetShuffleSpeed, 1.0f));

	float speedFactor = std::max(0.0f, std::min(1.0f, animState->feetShuffleSpeed2));

	float unk1 = ((animState->runningAccelProgress * -0.3f) - 0.1f) * speedFraction;
	float unk2 = unk1 + 1.0f;
	float delta;

	if (animState->duckProgress > 0) {
		unk2 += ((animState->duckProgress * speedFactor) * (0.5f - unk2));// - 1.f
	}

	delta = *(float *) ((uintptr_t) animState + 0x3A4) * unk2;

	return delta - 0.5f;
}

float legitAA(float first, float second, C_BasePlayer *player)
{
	if (first == second) {
		return 0.f;
	}

	float MaxDelta = GetMaxDeltaAP(player->GetAnimState());

	if ((first <= 0.f && second >= 0.f) || (first >= 0.f && second <= 0.f)) {
		return std::fabs(first - second);
	} //where the fuck is fabsf?

	if (first > MaxDelta && second < MaxDelta) {
		first -= (first - MaxDelta);
		second += (second + MaxDelta);
	} else if (first < MaxDelta && second > MaxDelta) {
		first += (first + MaxDelta);
		second -= (second - MaxDelta);
	}

	return std::fabs(first - second);
}

//dotq pasted most of this from his old project
//currently needs proper handling of incorrect pointers and such
//dotq: please kill me
//double thanks to Luna for the Resolver skeleton!

float angleDiff(float ang1, float ang2)
{
	ang1 = NormalizeYaw(ang1);
	ang2 = NormalizeYaw(ang2);

	return NormalizeYaw(ang1 - ang2);
}

static void Resolve(C_BasePlayer *player, float feetYaw, float angleYaw, CCSGOAnimState &animstate)
{
	/*
	//  Heyo! A message from @dotqurter / Echo here. If you'd like to help out with the resolver, a good place to start looking into how desync works is to think of it
	//  like this. When a player desyncs, they're abusing how the server animates every packet, while only sending the most "up to date" (bSendPacket == true!)
	//  "RunCommand" and "SetupVelocity" values. For example, while choking, you can abuse micromovements, LBY breaking and angle changing to essentially "set" your
	//  goalfeetyaw away from your "faked" goal feet yaw, by abusing the time it takes for your goalfeetyaw to update, so you "set" it on the enemy player's screen!
	//  This is the entire reason why desync works, glhf!
	//      -Also, if you want to assist in resolving, take a good look at how the server renders choked packets, and how the client never recieves them!
	*/

	/*
	//  Also, keep in mind, that LBY now updates on all ticks, not just sent ticks!
	*/
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!engine->IsInGame() || !player || !localplayer) {
		return;
	}

	auto AnimState = player->GetAnimState();

	if (!AnimState) {
		return;
	}

	float m_flMaxYaw = AnimState->unkown[516];
	float m_flMinYaw = AnimState->unkown[512];
	float lby = *player->GetLowerBodyYawTarget();
	float eye = player->GetEyeAngles()->y;
	float goalfeetyaw = AnimState->goalFeetYaw;
	float resolvedgoal = 0.f;
	float flYawModifier = (((AnimState->timeSinceStoppedMoving * -0.3f) - 0.2f) * AnimState->speed) + 1.0f;
	float flMaxYawModifier = m_flMaxYaw * flYawModifier;
	float flMinYawModifier = m_flMinYaw * flYawModifier;
	float eye_feet_delta = angleDiff(AnimState->yaw, resolvedgoal);
	float m_flLastClientSideAnimationUpdateTimeDelta = fabs(
			AnimState->lastAnimUpdateFrame - AnimState->lastAnimUpdateTime);

	float lbydelta = angleDiff(AnimState->yaw, lby);

	NormalizeYaw(lby);
	NormalizeYaw(eye);
	NormalizeYaw(goalfeetyaw);

	if (AnimState->duckProgress > 0.0f) {
		flYawModifier = flYawModifier + ((AnimState->duckProgress * AnimState->landingAnimationTimeLeftUntilDone) *
		                                 (0.5f - flYawModifier));
	}

	// Ducking/Landing increases the YawModifier, which is why desync appears further while ducking when using balance breaking desync / LBY.

	if (eye_feet_delta <= flMaxYawModifier) {
		if (flMinYawModifier > eye_feet_delta) {
			goalfeetyaw = fabs(flMinYawModifier) + eye;
		}
	} else //This is the infamous "Max Delta" that people rant about, which we cannot use the same methods as antiaim desync,
		// as our percieved AnimState of the enemy players is incorrect!
		// That's how desync works in the first place, and has nothing to do with eyeangles!
	{
		goalfeetyaw = eye - fabs(flMaxYawModifier);
	}
	// To be more precise with this function, we're verifying that player's eyeangles to goalfeetyaw delta isn't above it's limit.

	AnimState->currentFeetYaw = goalfeetyaw;
	//As is placed in SetupVelocity.

	resolvedgoal = NormalizeYaw(goalfeetyaw);
	//Here, we normalize the goalfeetyaw, as that's what SetupVelocity (which you should check out for proper resolving!) does.

	if (AnimState->speed < 0.1f || fabs(AnimState->absMovementDirection) < 100.0f) {
		resolvedgoal = ApproachAngle(eye, resolvedgoal, ((AnimState->duckProgress * 20.0f) + 30.0f) *
		                                                m_flLastClientSideAnimationUpdateTimeDelta);
	} else {
		resolvedgoal = ApproachAngle(lby, goalfeetyaw, m_flLastClientSideAnimationUpdateTimeDelta * 100.0f);
	}

	//if (layers->operator[](3).m_flWeight >= 0.6f)
	//   resolvedgoal -= legitAA(player->GetAnimState()->goalFeetYaw, player->GetAnimState()->yaw, player);

	if (lbydelta < -60.f) {
		resolvedgoal = (eye_feet_delta / m_flMinYaw) * lbydelta;
	}

	if (lbydelta > 60.f) {
		resolvedgoal = (eye_feet_delta / m_flMinYaw) * -lbydelta;
	}

	AnimState->goalFeetYaw = resolvedgoal;

	//cvar->ConsoleDPrintf("player lby delta: %f \n", lbydelta);

	memcpy(player->GetAnimState(), AnimState, sizeof(CCSGOAnimState));
}

void Resolver::FrameStageNotify(ClientFrameStage_t stage)
{
	if (!engine->IsInGame()) {
		return;
	}

	if (!Settings::Resolver::resolveAll) {
		return;
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer) {
		return;
	}

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		int maxClient = engine->GetMaxClients();
		for (int i = 1; i < maxClient; i++) {
			C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);
			// Resolver::AnimationFix(player);

			if (!player
			    || player == localplayer
			    || player->GetDormant()
			    || !player->IsAlive()
			    || player->GetImmune()
			    || Entity::IsTeamMate(player, localplayer)) {
				continue;
			}

			IEngineClient::player_info_t entityInformation;
			engine->GetPlayerInfo(i, &entityInformation);

			float trueDelta = NormalizeAsYaw(player->GetAnimState()->goalFeetYaw - player->GetEyeAngles()->y);

			if (player->GetVelocity().Length() > 30.f) {
				continue;
			}

			switch (Resolver::players[player->GetIndex()].MissedCount) {
				case 0:
					Resolve(player, player->GetAnimState()->goalFeetYaw, player->GetEyeAngles()->y,
					        *player->GetAnimState());
					break;
				case 1:
					break;
				case 2:
					Resolve(player, player->GetAnimState()->goalFeetYaw, player->GetEyeAngles()->y,
					        *player->GetAnimState());
					break;
				case 3:
					*player->GetLowerBodyYawTarget() = player->GetEyeAngles()->y + trueDelta;
					break;
				case 4:
					player->GetAnimState()->goalFeetYaw += 30;
					break;
				case 5:
					player->GetAnimState()->goalFeetYaw -= 30;
					break;
				case 6:
					static bool flip = false;
					flip ? player->GetEyeAngles()->y += 45 : player->GetEyeAngles()->y -= 45;
					flip = !flip;
					break;
				default:
					Resolver::players[player->GetIndex()].MissedCount = 0;
					break;
			}
			player->updateClientAnimation();
		}
	}
}

void Resolver::FireGameEvent(IGameEvent *event)
{
	// no need right now
}
