#pragma once

#include "../HeaderFiles.h"
#include "autowall.h"
#include "../TickManipulation/lagcomp.hpp"
#include "../../Utils/draw.h"

namespace Ragebot
{
	struct DATA
	{
		C_BasePlayer *player = nullptr;
		Vector PrevTickEyePosition = Vector(0);
		bool shooted = false;
		int playerhealth = 0;
		bool autoslow = false;
		QAngle prevAngle = QAngle(0);
		bool needToStop = false;
		bool Hitted = false;
		bool IHit = false;
	};

	typedef struct DATA Data;

	inline Data data;
	inline Vector localEye = Vector(0);
	inline Vector BestSpot = Vector(0);
	inline int BestDamage = 0;
	inline int ShootEnemyIndex = 0;
	inline int prevDamage = 0;
	inline matrix3x4_t BoneMatrix[128];

	void CreateMove(CUserCmd *);

	void FireGameEvent(IGameEvent *event);

	void FireGameEvent2(IGameEvent *event);

	/*
	 * Yo
	 */
	inline C_BasePlayer *enemy = nullptr;
	inline C_BasePlayer *localplayer = nullptr;
	inline C_BaseCombatWeapon *activeWeapon = nullptr;
	inline Vector bulPosition;
	inline RageWeapon_t *currentWeaponSetting;

	void GetDamageAndSpots(C_BasePlayer *player, Vector &Spot, int &Damage, const int playerhealth, int i,
	                       const std::unordered_map<int, int> *modelType, matrix3x4_t bonematrix[]);

	void BestHeadPoint(C_BasePlayer *player, const int &BoneIndex, int &Damage, Vector &Spot);

	void BestMultiPoint(C_BasePlayer *player, const int &BoneIndex, int &Damage, Vector &Spot);

	void GetBestSpotAndDamage(C_BasePlayer *player, Vector &Spot, int &Damage);

	bool
	canShoot(C_BaseCombatWeapon *activeWeapon, C_BasePlayer *enemy, QAngle angle, const RageWeapon_t &currentSettings);

	void AutoShoot(C_BasePlayer *enemy, CUserCmd *cmd, QAngle angle, RageWeapon_t *currentSettings);

	C_BasePlayer *GetClosestEnemy(C_BasePlayer *localplayer);

	void GetBestEnemy();

	void CheckHit();

	void SetBulletPositions(float &x, float &y, float &z);

	void clearBulletPositions();

	void init(C_BasePlayer *, C_BaseCombatWeapon *);
}

// extern WriteUserCmdFn WriteUserCmd;
