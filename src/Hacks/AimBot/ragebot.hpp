#pragma once

#include "../HeaderFiles.h"
#include "autowall.h"
#include "../TickManipulation/lagcomp.hpp"
#include "../../Utils/draw.h"

class RagebotPredictionSystem{

private:

	C_BasePlayer* enemy = nullptr;
	C_BasePlayer* localplayer = nullptr;
    C_BaseCombatWeapon* activeWeapon = nullptr;

    #define RandomeFloat(x) (static_cast<double>( static_cast<double>(std::rand())/ static_cast<double>(RAND_MAX/x)))
    
    // items to check hit Bullet_Impact
    std::vector<Vector> bulletPosition;

	void GetDamageAndSpots(C_BasePlayer* player, Vector &Spot, int& Damage, int& playerHelth, int& i,const std::unordered_map<int, int>* modelType, const RageWeapon_t& currentSetting);

	void BestHeadPoint(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot);

    void BestMultiPoint(C_BasePlayer* player, int &BoneIndex, int& Damage, Vector& Spot);

	void GetBestSpotAndDamage(C_BasePlayer* player, Vector& Spot, int& Damage,const RageWeapon_t& currSettings);
public :

    RagebotPredictionSystem(){}

	bool canShoot(C_BaseCombatWeapon*,Vector&, C_BasePlayer*,const RageWeapon_t&);

	void GetBestEnemy(const RageWeapon_t&);

    void CheckHit();

    void SetBulletPositions(float& x, float& y, float& z);
    void clearBulletPositions();

    void init(C_BasePlayer*, C_BaseCombatWeapon*);

    // Vector GetBulletPosition(){
    //     return bulletPosition;
    // }

	C_BasePlayer* GetEnemy();
};


namespace Ragebot {
    struct DATA 
    {
        C_BasePlayer* player = nullptr;
        Vector PrevTickEyePosition = Vector(0);
        Vector BestSpot = Vector(0);
        bool shooted = false;
        int playerhelth = 0;
        bool slowWalking = false;
        QAngle prevAngle = QAngle(0);
    };

    inline struct DATA data;
    inline Vector localEye = Vector(0);
    inline Vector BestSpot = Vector(0);
    inline int BestDamage = 0;
    extern std::vector<int64_t> friends;

    inline int prevDamage = 0;
    inline RagebotPredictionSystem* ragebotPredictionSystem = new RagebotPredictionSystem();

    void CreateMove(CUserCmd*);
    void FireGameEvent(IGameEvent* event);
}
