#include "legitbot.hpp"
#include "autowall.h"
#include "aimbot.hpp"
#include "../aimstep.h"

std::vector<int64_t> Legitbot::friends = {};
std::vector<long> killTimes = {0}; // the Epoch time from when we kill someone

static QAngle lastRandom = QAngle(0);
static QAngle LastPunch = QAngle(0);

static bool shouldAim = false;
int Legitbot::targetAimbot = -1;

static QAngle oldAngle = QAngle(0);
static QAngle angle = QAngle(0);

static bool IsInFov(const Vector *spot, float Fov)
{
	Vector pVecTarget = Legitbot::localplayer->GetEyePosition();

	if (fabs(Math::GetFov(oldAngle, Math::CalcAngle(pVecTarget, *spot))) > Fov) {
		return false;
	}

	return true;
}

static C_BasePlayer *GetClosestEnemy(const LegitWeapon_t *currentSettings)
{
	float bestFov = currentSettings->LegitautoAimFov;

	Vector pVecTarget = Legitbot::localplayer->GetEyePosition();
	C_BasePlayer *enemy = nullptr;

	int maxClient = engine->GetMaxClients();
	for (int i = 1; i < maxClient; i++) {
		C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);

		if (!player
		    || player == Legitbot::localplayer
		    || player->GetDormant()
		    || !player->IsAlive()
		    || player->GetImmune()
		    || Entity::IsTeamMate(player, Legitbot::localplayer)) {
			continue;
		}

		Vector cbVecTarget = player->GetEyePosition();

		float cbFov = fabs(Math::GetFov(oldAngle, Math::CalcAngle(pVecTarget, cbVecTarget)));

		if (cbFov > bestFov) {
			continue;
		}

		bestFov = cbFov;
		enemy = player;
		AutoWall::targetAimbot = i;
	}

	if (enemy && !enemy->SetupBones(Legitbot::BoneMatrix, 128, 256, 0)) {
		return nullptr;
	}

	return enemy;
}

static void Salt(float &smooth, const LegitWeapon_t *currentSettings)
{
	float sin = std::sin(globalVars->tickcount);
	float salt = sin * currentSettings->smoothSaltMultiplier;
	smooth *= (smooth + salt);
}

static void Smooth(C_BasePlayer *player, QAngle &angle, const QAngle &oldAngle, const LegitWeapon_t *currentSettings)
{
	QAngle delta = angle - oldAngle;
	Math::NormalizeAngles(delta);

	float smooth = powf(currentSettings->smoothAmount, 0.4f); // Makes more slider space for actual useful values

	smooth = std::min(0.99f, smooth);

	if (currentSettings->smoothSaltEnabled) {
		Salt(smooth, currentSettings);
	}

	QAngle toChange = {0, 0, 0};

	SmoothType type = currentSettings->smoothType;

	if (type == SmoothType::SLOW_END) {
		toChange = delta - (delta * smooth);
	} else {
		float coeff = (1.0f - smooth) / delta.Length() * 4.f;

		if (type == SmoothType::FAST_END) {
			coeff = powf(coeff, 2.f) * 10.f;
		}

		coeff = std::min(1.f, coeff);
		toChange = delta * coeff;
	}

	angle = oldAngle + toChange;
}

static void AutoShoot(C_BaseCombatWeapon *activeWeapon, CUserCmd *cmd, LegitWeapon_t *currentSettings)
{
	if (!activeWeapon || activeWeapon->GetInReload()) {
		return;
	}
	if (currentSettings->autoScopeEnabled && Util::Items::IsScopeable(*activeWeapon->GetItemDefinitionIndex()) &&
	    !Legitbot::localplayer->IsScoped() && !(cmd->buttons & IN_ATTACK2) && !(cmd->buttons & IN_ATTACK)) {
		cmd->buttons |= IN_ATTACK2;
		return;
	}

	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_KNIFE || weaponType == CSWeaponType::WEAPONTYPE_C4 ||
	    weaponType == CSWeaponType::WEAPONTYPE_GRENADE) {
		return;
	}
	if (*activeWeapon->GetItemDefinitionIndex() == ItemDefinitionIndex::WEAPON_REVOLVER) {
		return;
	}
	if (Aimbot::canShoot(Legitbot::localplayer, activeWeapon, currentSettings->hitchance)) {
		if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime) {
			cmd->buttons &= ~IN_ATTACK;
		} else {
			cmd->buttons |= IN_ATTACK;
		}
		return;
	}

	Aimbot::AutoSlow(Legitbot::localplayer, cmd, currentSettings->autoSlow);
}

static void GetClosestSpot(C_BasePlayer *enemy, Vector &BestSpot, const std::unordered_map<int, int> *modelType,
                           const LegitWeapon_t *currentSettings)
{
	float bestFov = currentSettings->LegitautoAimFov;

	int len = 31;

	for (int i = 0; i < len; i++) {
		int boneID = (*modelType).at(i);

		if (boneID == BONE_INVALID) {
			continue;
		}

		Vector bone3D = enemy->GetBonePosition(boneID, Legitbot::BoneMatrix);

		if (!currentSettings->mindamage) {
			Vector pVecTarget = Legitbot::localplayer->GetEyePosition();
			float cbFov = fabs(Math::GetFov(oldAngle, Math::CalcAngle(pVecTarget, bone3D)));

			if (cbFov <= bestFov) {
				bestFov = cbFov;
				BestSpot = bone3D;
			}
			continue;
		}

		if (!IsInFov(&bone3D, bestFov)) { continue; }

		int boneDamage = AutoWall::GetDamage(bone3D, true);
		int PrevDamage = 0;
		int playerhealth = enemy->GetHealth();
		if (boneDamage >= playerhealth) {
			BestSpot = bone3D;
			return;
		} else if (boneDamage >= currentSettings->minDamagevalue && boneDamage > PrevDamage) {
			BestSpot = bone3D;
			PrevDamage = boneDamage;
		}
	}
}

static C_BasePlayer *GetClosestPlayerAndSpot(CUserCmd *cmd, bool visibleCheck, Vector &bestSpot, float &bestDamage,
                                             const LegitWeapon_t *currentSettings)
{
	if (!currentSettings->autoAimEnabled) { return nullptr; }
	if (Legitbot::localplayer->IsFlashed()) { return nullptr; }

	C_BasePlayer *player = nullptr;
	player = GetClosestEnemy(currentSettings); // getting the closest enemy to the crosshair

	if (!player || !player->IsAlive()) { return nullptr; }

	const std::unordered_map<int, int> *modelType = BoneMaps::GetModelTypeBoneMap(player);

	Vector bone3d;

	if (currentSettings->predEnabled) {
		int BoneId = (*modelType).at(currentSettings->bone);
		bone3d = player->GetBonePosition(BoneId, Legitbot::BoneMatrix);

		if (IsInFov(&bone3d, currentSettings->LegitautoAimFov)) {
			goto ReadyToShoot;
		} else {
			goto CheckForClosestSPot;
		}
	}

	CheckForClosestSPot:
	GetClosestSpot(player, bone3d, modelType, currentSettings);

	ReadyToShoot:
	bestSpot = bone3d;

	if (LineGoesThroughSmoke(Legitbot::localplayer->GetEyePosition(), bone3d, true)) {
		return nullptr;
	}

	if (AutoWall::GetDamage(bone3d, true) <= 0) {
		return nullptr;
	}
	if (!currentSettings->autoWall && !Entity::IsSpotVisibleThroughEnemies(player, bone3d)) {
		return nullptr;
	}

	return player;
}

void Legitbot::CreateMove(CUserCmd *cmd)
{
	localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!Legitbot::localplayer || !Legitbot::localplayer->IsAlive()) {
		return;
	}
	activeWeapon = (C_BaseCombatWeapon *) entityList->GetClientEntityFromHandle(
			Legitbot::localplayer->GetActiveWeapon());
	if (!activeWeapon || activeWeapon->GetInReload() || activeWeapon->GetAmmo() == 0) {
		return;
	}
	if (activeWeapon->GetNextPrimaryAttack() > globalVars->curtime) {
		return;
	}
	CSWeaponType weaponType = activeWeapon->GetCSWpnData()->GetWeaponType();
	if (weaponType == CSWeaponType::WEAPONTYPE_C4 || weaponType == CSWeaponType::WEAPONTYPE_GRENADE ||
	    weaponType == CSWeaponType::WEAPONTYPE_KNIFE) {
		return;
	}

	Vector bestSpot = Vector(0);
	float bestDamage = float(0);
	Vector localEye = Legitbot::localplayer->GetEyePosition();

	angle = cmd->viewangles;
	engine->GetViewAngles(oldAngle);
	shouldAim = false;

	ItemDefinitionIndex index = ItemDefinitionIndex::INVALID;
	if (Settings::Legitbot::weapons.find(*activeWeapon->GetItemDefinitionIndex()) !=
	    Settings::Legitbot::weapons.end()) {
		index = *activeWeapon->GetItemDefinitionIndex();
	}
	currentWeaponSetting = &Settings::Legitbot::weapons.at(index);

	if (currentWeaponSetting->ignoreJumpEnabled &&
	    (!(Legitbot::localplayer->GetFlags() & FL_ONGROUND) &&
	     Legitbot::localplayer->GetMoveType() != MOVETYPE_LADDER)) {
		return;
	}

	C_BasePlayer *player = GetClosestPlayerAndSpot(cmd, true, bestSpot, bestDamage, currentWeaponSetting);

	if (player) {
		// cvar->ConsoleDPrintf(XORSTR("Found Player"));
		if (currentWeaponSetting->aimkeyOnly && inputSystem->IsButtonDown(currentWeaponSetting->aimkey)) {
			shouldAim = true;
		} else if (cmd->buttons & IN_ATTACK) {
			shouldAim = true;
		} else if (currentWeaponSetting->autoShoot) {
			shouldAim = true;
		} else {
			goto DoNothing;
		}

		Settings::Debug::AutoAim::target = bestSpot; // For Debug showing aimspot.

		if (shouldAim) {
			if (currentWeaponSetting->predEnabled) {
				Aimbot::VelocityExtrapolate(player, bestSpot);
			} // trying to assume next position of the player

			angle = Math::CalcAngle(localEye, bestSpot);

			if (currentWeaponSetting->errorMarginEnabled) {
				static int lastShotFired = 0;
				if ((Legitbot::localplayer->GetShotsFired() >
				     lastShotFired)) { //get new random spot when firing a shot or when aiming at a new target
					lastRandom = Aimbot::ApplyErrorToAngle(&angle, currentWeaponSetting->errorMarginValue);
				}

				angle += lastRandom;
				lastShotFired = Legitbot::localplayer->GetShotsFired();
			}
			if (currentWeaponSetting->smoothEnabled) { Smooth(player, angle, oldAngle, currentWeaponSetting); }
			if (currentWeaponSetting->autoShoot) { AutoShoot(activeWeapon, cmd, currentWeaponSetting); }
			AimStep::Run(angle, cmd, currentWeaponSetting);
		}
	} else // No player to Shoot
	{
		Settings::Debug::AutoAim::target.Zero();
		lastRandom.Init(0.f, 0.f, 0.f);
		if (cmd->buttons & IN_ATTACK) { angle = oldAngle; }
	}

	DoNothing:
	Aimbot::NoRecoil(angle, cmd, Legitbot::localplayer, activeWeapon, currentWeaponSetting->silent);
	Aimbot::AutoPistol(activeWeapon, cmd, currentWeaponSetting->autoPistolEnabled);

	Math::NormalizeAngles(angle);
	Math::ClampAngles(angle);

	Aimbot::FixMouseDeltas(cmd, angle, oldAngle);

	if (cmd->buttons & IN_ATTACK) {
		cmd->viewangles = angle;
	}

	if (!currentWeaponSetting->silent && CreateMove::sendPacket) {
		engine->SetViewAngles(angle);
	}
}

void Legitbot::FireGameEvent(IGameEvent *event)
{
	if (!Settings::Legitbot::enabled || !event) {
		return;
	}

	if (strcmp(event->GetName(), XORSTR("player_connect_full")) == 0 ||
	    strcmp(event->GetName(), XORSTR("cs_game_disconnected")) == 0) {
		if (event->GetInt(XORSTR("userid")) &&
		    engine->GetPlayerForUserID(event->GetInt(XORSTR("userid"))) != engine->GetLocalPlayer()) {
			return;
		}
		Legitbot::friends.clear();
	}
	if (strcmp(event->GetName(), XORSTR("player_death")) == 0) {
		int attacker_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("attacker")));
		int deadPlayer_id = engine->GetPlayerForUserID(event->GetInt(XORSTR("userid")));

		if (attacker_id == deadPlayer_id) { // suicide
			return;
		}

		if (attacker_id != engine->GetLocalPlayer()) {
			return;
		}

		killTimes.push_back(Util::GetEpochTime());
	}
}

