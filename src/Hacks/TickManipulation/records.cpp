#include "records.hpp"

#ifndef CLAMP
#define CLAMP(x, upper, lower) (std::min(upper, std::max(x, lower)))
#endif

float Records::GetLerpTime()
{
	int updateRate = cvar->FindVar("cl_updaterate")->GetInt();
	ConVar *minUpdateRate = cvar->FindVar("sv_minupdaterate");
	ConVar *maxUpdateRate = cvar->FindVar("sv_maxupdaterate");

	if (minUpdateRate && maxUpdateRate) {
		updateRate = maxUpdateRate->GetInt();
	}

	float ratio = cvar->FindVar("cl_interp_ratio")->GetFloat();

	if (ratio == 0) {
		ratio = 1.0f;
	}

	float lerp = cvar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = cvar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = cvar->FindVar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1) {
		ratio = CLAMP(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());
	}

	return std::max(lerp, (ratio / updateRate));
}

bool Records::IsTickValid(float time) // pasted from polak getting some invalid ticks need some fix
{
	float correct = 0;

	correct += Records::GetLerpTime();
	correct = CLAMP(correct, 0.f, cvar->FindVar("sv_maxunlag")->GetFloat());

	float deltaTime = correct - (globalVars->curtime - time);

	if (fabsf(deltaTime) < 0.2f) {
		return true;
	}

	return false;
}

void Records::RemoveInvalidTicks()
{
	auto &Ticks = Records::Ticks;

	for (auto Tick = Ticks.begin(); Tick != Ticks.end(); Tick++) {
		if (!Records::IsTickValid(Tick->simulationTime)) {
			Ticks.erase(Tick);

			if (!Ticks.empty()) {
				Tick = Ticks.begin();
			} else {
				break;
			}
		}
	}
}

static bool GetBestSpotDamage(C_BasePlayer *enemy, Vector &BestSpot, int &BestDamage, matrix3x4_t BoneMatrix[])
{
	if (!Ragebot::currentWeaponSetting) {
		return false;
	}

	bool gotBestSpot;
	gotBestSpot = false;

	int boneID = -1;

	for (int boneIndex = 0; boneIndex < 6; boneIndex++) {
		Vector bestspot = Vector(0);
		int bestDamage = 0;

		if (!Ragebot::currentWeaponSetting->desireBones[boneIndex]) { continue; }

		const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(enemy);

		switch ((DesireBones) boneIndex) {
			case DesireBones::BONE_HEAD:
				boneID = (*modelType).at(BONE_HEAD);
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;

			case DesireBones::UPPER_CHEST:
				boneID = (*modelType).at(BONE_UPPER_SPINAL_COLUMN);
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;

			case DesireBones::MIDDLE_CHEST:
				boneID = (*modelType).at(BONE_MIDDLE_SPINAL_COLUMN);
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;

			case DesireBones::LOWER_CHEST:
				boneID = (*modelType).at(BONE_LOWER_SPINAL_COLUMN);
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;

			case DesireBones::BONE_HIP:
				boneID = (*modelType).at(BONE_HIP);
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;

			case DesireBones::LOWER_BODY:
				boneID = BONE_PELVIS;
				bestspot = enemy->GetBonePosition(boneID, BoneMatrix);
				bestDamage = AutoWall::GetDamage(bestspot, true);
				break;
		}
		// Ragebot::GetDamageAndSpots(enemy, bestspot, bestDamage, enemy->GetHealth(), boneIndex, modelType, BoneMatrix);

		if (bestDamage >= enemy->GetHealth()) {
			BestSpot = bestspot;
			return true;
		} else {
			if (inputSystem->IsButtonDown(Settings::Ragebot::DamageOverrideBtn)) {
				if (bestDamage >= Ragebot::currentWeaponSetting->DamageOverride && bestDamage > BestDamage) {
					BestDamage = bestDamage;
					BestSpot = bestspot;
					gotBestSpot = true;
				}

			} else if (bestDamage >= Ragebot::currentWeaponSetting->MinDamage && bestDamage > BestDamage) {
				BestDamage = bestDamage;
				BestSpot = bestspot;
				gotBestSpot = true;
			}
		}
	}
	return gotBestSpot;
}

void Records::RegisterTicks()
{
	const auto localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	const auto curTick = Records::Ticks.insert(Records::Ticks.begin(), {globalVars->tickcount, globalVars->curtime});

	int maxClients = engine->GetMaxClients();
	for (int i = maxClients; i > 1; i--) {
		const auto player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player
		    || player == localplayer
		    || player->GetDormant()
		    || !player->IsAlive()
		    || Entity::IsTeamMate(player, localplayer)
		    || player->GetImmune()) {
			continue;
		}

		Records::Record record;

		if (!player->SetupBones(record.bone_matrix, 128, BONE_USED_BY_HITBOX, globalVars->curtime)) {
			continue;
		}

		record.entity = player;
		record.simulationTime = player->GetSimulationTime();
		record.origin = player->GetAbsOrigin();
		record.head = player->GetBonePosition(BONE_HEAD);

		curTick->records.push_back(record);

		Vector BestSpot = Vector(0);
		int BestDamage;
		BestDamage = 0;

		if (GetBestSpotDamage(player, BestSpot, BestDamage, record.bone_matrix)) {
			if (BestDamage >= curTick->bestenemy.BestDamage) {
				curTick->bestenemy.entity = player;
				curTick->bestenemy.BestSpot = BestSpot;
				BestDamage = curTick->bestenemy.BestDamage;
			}
		}
	}
}

void Records::RemoveBackTrackTicks(const int &maxTick)
{
	int size;
	size = Records::Ticks.size() - 1;
	if (size >= maxTick) {
		Records::Ticks.erase(Ticks.begin());
	}
}
