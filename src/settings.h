#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "ImGUI/imgui.h"
#include "Utils/util_sdk.h"
#include "Utils/util_items.h"
#include "Utils/util.h"

#include "SDK/IClientEntity.h"
#include "SDK/IInputSystem.h"
#include "SDK/definitions.h"
#include "SDK/Materialsystem_config.h"

enum class PlayerModel : int
{
	Ricksaw,
	Operator,
	Ava,
	SealTeam6Sodier,
	thirdCommandoCompany,
	BSquadronOfficer,
	ChemHazSpecialist,
	BioHazSpecialist,
	Enforcer,
	Soldier,
	GroundRebel,
	StreetSoldier,
	Dragomir,
	MarkusDelrow,
	Buckshot,
	John,
	SergeantBombson,
	BlueberriesBuckshot,
	NONE,
};

enum class UiType : int
{
	LagacyMenu,
	AimwareV5,
};
enum class DsyncType : int
{
	type1,
	type2,
};
enum class DesireBones : int
{
	BONE_HEAD = 0,
	UPPER_CHEST,
	MIDDLE_CHEST,
	LOWER_CHEST,
	BONE_HIP,
	LOWER_BODY,
};
enum class DrawingBackend : int
{
	SURFACE = 0,
	IMGUI,
	NUMBER_OF_TYPES,
};
enum class SmoothType : int
{
	SLOW_END,
	CONSTANT,
	FAST_END,
};
enum class TracerType : int
{
	BOTTOM,
	CURSOR,
};
enum class ClanTagType : int
{
	STATIC,
	MARQUEE,
	WORDS,
	LETTERS,
};
enum class AutostrafeType : int
{
	AS_FORWARDS,
	AS_BACKWARDS,
	AS_LEFTSIDEWAYS,
	AS_RIGHTSIDEWAYS,
	AS_RAGE,
};
enum class ChamsType : int
{
	WHITEADDTIVE,
	ADDITIVETWO,
	WIREFRAME,
	FLAT,
	PEARL,
	GLOW,
	GLOWF,
	NONE,
};
enum class Filter : int
{
	Enemies,
	LocalPlayer,
	Alise,
	Fish,
	Chicken,
};
enum class BoxType : int
{
	FLAT_2D,
	FRAME_2D,
	BOX_3D,
	HITBOXES,
};
enum class SpriteType : int
{
	SPRITE_TUX,
};
enum class Sound : int
{

	NONE,
	SPONGEBOB,
	HALFLIFE,
	HALFLIFE2,
	HALFLIFE3,
	HALFLIFE4,
	BBGUNBELL,
	DOPAMINE,
	WUB,
	PEDOYES,
	MEME,
	ERROR,
	ORCHESTRAL,

};
enum class BarType : int
{
	VERTICAL,
	VERTICAL_RIGHT,
	HORIZONTAL,
	HORIZONTAL_UP,
	INTERWEBZ,
};
enum class BarColorType : int
{
	STATIC_COLOR,
	HEALTH_BASED,
};
enum class TeamColorType : int
{
	ABSOLUTE,
	RELATIVE,
};
enum class SmokeType : int
{
	WIREFRAME,
	NONE,
};
enum class AimTargetType : int
{
	FOV,
	DISTANCE,
	REAL_DISTANCE,
	HP,
};
enum class SpammerType : int
{
	SPAMMER_NONE,
	SPAMMER_NORMAL,
	SPAMMER_POSITIONS,
};
enum class AntiAimRealType_Y : int
{
	NONE,
	Static,
	Jitter,
};
enum class AntiAimFakeType_y : int
{
	NONE,
	Static,
	Jitter,
	Randome,
};
enum class SkinAndModel : int
{
	Skins,
	Model,
};
enum class HitchanceType : int
{
	Normal = 0,
	ForceAccuracy,
};

struct LegitWeapon_t
{
	bool silent,
			autoShoot,
			aimkeyOnly,
			smoothEnabled,
			smoothSaltEnabled,
			errorMarginEnabled,
			autoAimEnabled,
			aimStepEnabled,
			rcsEnabled,
			rcsAlwaysOn,
			hitchanceEnabled,
			autoPistolEnabled,
			autoScopeEnabled,
			ignoreJumpEnabled,
			ignoreEnemyJumpEnabled,
			autoSlow,
			predEnabled,
			TriggerBot,
			mindamage,
			autoWall,
			PriorityBone;
	Bone bone = BONE_HEAD;
	SmoothType smoothType = SmoothType::SLOW_END;
	ButtonCode_t aimkey = ButtonCode_t::MOUSE_MIDDLE;
	float smoothAmount = 1.0f,
			smoothSaltMultiplier = 0.0f,
			errorMarginValue = 0.0f,
			LegitautoAimFov = 15.0f,
			aimStepMin = 25.0f,
			aimStepMax = 35.0f,
			rcsAmountX = 2.0f,
			rcsAmountY = 2.0f,
			minDamagevalue = 10.0f,
			hitchance = 20;
	bool desiredBones[31];

	bool operator==(const LegitWeapon_t &another) const
	{
		for (int bone = BONE_PELVIS; bone <= BONE_RIGHT_SOLE; bone++) {
			if (this->desiredBones[bone] != another.desiredBones[bone]) {
				return false;
			}
		}

		return this->silent == another.silent &&
		       this->autoShoot == another.autoShoot &&
		       this->bone == another.bone &&
		       this->aimkey == another.aimkey &&
		       this->aimkeyOnly == another.aimkeyOnly &&
		       this->smoothEnabled == another.smoothEnabled &&
		       this->smoothAmount == another.smoothAmount &&
		       this->smoothType == another.smoothType &&
		       this->smoothSaltEnabled == another.smoothSaltEnabled &&
		       this->smoothSaltMultiplier == another.smoothSaltMultiplier &&
		       this->errorMarginEnabled == another.errorMarginEnabled &&
		       this->errorMarginValue == another.errorMarginValue &&
		       this->autoAimEnabled == another.autoAimEnabled &&
		       this->LegitautoAimFov == another.LegitautoAimFov &&
		       this->aimStepEnabled == another.aimStepEnabled &&
		       this->aimStepMin == another.aimStepMin &&
		       this->aimStepMax == another.aimStepMax &&
		       this->rcsEnabled == another.rcsEnabled &&
		       this->rcsAlwaysOn == another.rcsAlwaysOn &&
		       this->rcsAmountX == another.rcsAmountX &&
		       this->rcsAmountY == another.rcsAmountY &&
		       this->autoPistolEnabled == another.autoPistolEnabled &&
		       this->autoScopeEnabled == another.autoScopeEnabled &&
		       this->ignoreEnemyJumpEnabled == another.ignoreEnemyJumpEnabled &&
		       this->hitchanceEnabled == another.hitchanceEnabled &&
		       this->hitchance == another.hitchance &&
		       this->autoSlow == another.autoSlow &&
		       this->predEnabled == another.predEnabled &&
		       this->TriggerBot == another.TriggerBot &&
		       this->mindamage == another.mindamage &&
		       this->autoWall == another.autoWall;
	}
} const defaultSettings{};
struct RageWeapon_t
{
	bool silent = false,
		 friendly = false,
		 closestBone = false,
		 autoPistolEnabled = false,
		 autoShootEnabled = false,
		 autoScopeEnabled = false,
		 aimStepEnabled = false,
		 autoSlow = false,
		 scopeControlEnabled = false,
		 DoubleFire = false,
		 AutoCrouch = false,
		 OnShot = false,
		 OnShotOnKey = false;
	float MinDamage = 50.f,
			DamageOverride = 100.f,
			aimStepMin = 28.0f,
			aimStepMax = 29.0f;
	int HitChance = 50;
	bool desireBones[6];
	bool desiredMultiBones[6];
	HitchanceType hitchanceType = HitchanceType::Normal;

	bool operator==(const RageWeapon_t &Ragebotanother) const
	{
		for (int bone = 0; bone < 6; bone++) // static bones
		{
			if (this->desireBones[bone] != Ragebotanother.desireBones[bone]) {
				return false;
			}
			if (this->desiredMultiBones[bone] != Ragebotanother.desiredMultiBones[bone]) {
				return false;
			}
		}

		return this->silent == Ragebotanother.silent &&
		       this->friendly == Ragebotanother.friendly &&
		       this->closestBone == Ragebotanother.closestBone &&
		       this->autoPistolEnabled == Ragebotanother.autoPistolEnabled &&
		       this->autoShootEnabled == Ragebotanother.autoShootEnabled &&
		       this->autoScopeEnabled == Ragebotanother.autoScopeEnabled &&
		       this->aimStepEnabled == Ragebotanother.aimStepEnabled &&
		       this->aimStepMin == Ragebotanother.aimStepMin &&
		       this->aimStepMax == Ragebotanother.aimStepMax &&
		       this->MinDamage == Ragebotanother.MinDamage &&
		       this->autoSlow == Ragebotanother.autoSlow &&
		       this->scopeControlEnabled == Ragebotanother.scopeControlEnabled &&
		       this->HitChance == Ragebotanother.HitChance &&
		       this->DoubleFire == Ragebotanother.DoubleFire &&
		       this->AutoCrouch == Ragebotanother.AutoCrouch &&
		       this->DamageOverride == Ragebotanother.DamageOverride &&
		       this->hitchanceType == Ragebotanother.hitchanceType &&
		       this->OnShot == Ragebotanother.OnShot;
	}

} const ragedefault{};

class ColorVar
{
public:
	ImColor color;
	ImVec4 ColorVec4;
	bool rainbow;
	float rainbowSpeed;

	ColorVar()
	{}

	ColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->ColorVec4 = getVec4();
	}

	ImColor Color()
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : this->color;
		result.Value.w = this->color.Value.w;
		return result;
	}

	ImVec4 getVec4()
	{
		return color.operator ImVec4();
	}
};

class HealthColorVar : public ColorVar
{
public:
	bool hp;

	HealthColorVar(ImColor color)
	{
		this->color = color;
		this->rainbow = false;
		this->rainbowSpeed = 0.5f;
		this->hp = false;
	}

	ImColor Color(C_BasePlayer *player)
	{
		ImColor result = this->rainbow ? Util::GetRainbowColor(this->rainbowSpeed) : (this->hp ? Color::ToImColor(
				Util::GetHealthColor(player)) : this->color);
		result.Value.w = this->color.Value.w;
		return result;
	}
};

namespace Settings
{
	namespace UI
	{
		inline ColorVar mainColor = ImColor(42, 45, 42, 255);
		inline ColorVar bodyColor = ImColor(0, 0, 0, 245);
		inline ColorVar fontColor = ImColor(238, 224, 224, 255);
		inline ColorVar accentColor = ImColor(0, 171, 178, 255);
		inline bool imGuiAliasedLines = false;
		inline bool imGuiAliasedFill = true;
		inline UiType uitype = UiType::AimwareV5;

		/* Window Position/Size Defaults */
		namespace Windows
		{
			namespace Colors
			{
				inline int posX = 540;
				inline int posY = 325;
				inline int sizeX = 540;
				inline int sizeY = 360;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Properties.
			}
			namespace Config
			{
				inline int posX = 185;
				inline int posY = 250;
				inline int sizeX = 185;
				inline int sizeY = 250;
				inline bool open = false;
				inline bool reload = false;
			}
			namespace Main
			{
				inline int posX = 30;
				inline int posY = 20;
				inline int sizeX = 960;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Playerlist
			{
				inline int posX = 700;
				inline int posY = 500;
				inline int sizeX = 700;
				inline int sizeY = 500;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Skinmodel
			{
				inline int posX = 1050;
				inline int posY = 645;
				inline int sizeX = 1050;
				inline int sizeY = 645;
				inline bool open = false;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
			namespace Spectators
			{
				inline int posX = 50;
				inline int posY = 100;
				inline int sizeX = 50;
				inline int sizeY = 100;
				inline bool reload = false; // True on config load, used to change Window Position.
			}
		}
		namespace Fonts
		{
			namespace ESP
			{
				inline char *family = (char *) "Georgia";
				inline int size = 12;
				inline int flags = (int) FontFlags::FONTFLAG_OUTLINE;
			}
		}
	}
	/* Default LegitBot Settings */
	namespace Legitbot
	{
		inline bool enabled = false;

		inline std::unordered_map<ItemDefinitionIndex, LegitWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons = {
				{ItemDefinitionIndex::INVALID, defaultSettings},
		};
	}

	namespace Ragebot
	{
		inline bool enabled = false;
		inline ButtonCode_t DamageOverrideBtn = ButtonCode_t::MOUSE_MIDDLE;
		inline ButtonCode_t OnShotBtn = ButtonCode_t::KEY_5;
		inline std::unordered_map<ItemDefinitionIndex, RageWeapon_t, Util::IntHash<ItemDefinitionIndex>> weapons = {
				{ItemDefinitionIndex::INVALID, ragedefault},
		};
	}

	namespace LagComp
	{
		inline bool enabled = false;
	}

	namespace BackTrack
	{
		inline bool enabled = false;
	}

	namespace Triggerbot
	{
		inline bool enabled = false;
		namespace OnKey
		{
			inline bool enable = true;
			inline ButtonCode_t key = ButtonCode_t::KEY_LALT;
		}

		namespace Magnet
		{
			inline bool enabled = false;
		}

		namespace Filters
		{
			inline bool enemies = true;
			inline bool allies = false;
			inline bool walls = false;
			inline bool smokeCheck = false;
			inline bool flashCheck = false;
			inline bool head = true;
			inline bool chest = true;
			inline bool stomach = true;
			inline bool arms = true;
			inline bool legs = true;
		}

		namespace RandomDelay
		{
			inline bool enabled = false;
			inline int lowBound = 20; // in ms
			inline int highBound = 35;// in ms
			inline int lastRoll = 0;
		}
	}

	namespace AntiAim
	{
		inline bool Enabled = false;
		inline bool EnableFakAngle = false;
		inline bool inverted = false;
		inline bool ShowReal = false;
		inline bool InvertOnShoot = false;
		inline bool atTheTarget = false;
		inline bool PitchDown = false;
		inline bool JitterFake = false;
		inline bool autoDirection = false;
		inline float offset = 0.f;
		inline float NetFake = 0.f;
		inline DsyncType dsynctype = DsyncType::type1;

		inline ButtonCode_t InvertKey = ButtonCode_t::KEY_T;

		namespace Jitter
		{
			inline int Value = 0.f;
			inline bool SyncWithLag = true;
		}
		namespace ManualAntiAim
		{
			inline bool Enable = false;
			inline ButtonCode_t backButton = ButtonCode_t::KEY_X;
			inline ButtonCode_t RightButton = ButtonCode_t::KEY_C;
			inline ButtonCode_t LeftButton = ButtonCode_t::KEY_Z;
		}
		namespace HeadEdge
		{
			inline bool enabled = false;
		}

		namespace FakeDuck
		{
			inline bool enabled = false;
			inline ButtonCode_t fakeDuckKey = ButtonCode_t::KEY_LCONTROL;
		}

		namespace FakeWalk
		{
			inline bool enabled = false;
			inline ButtonCode_t Key = ButtonCode_t::KEY_LSHIFT;
			inline float Speed = 50;
		}
		namespace SlowWalk
		{
			inline bool enabled = false;
			inline ButtonCode_t Key = ButtonCode_t::KEY_LSHIFT;
			inline float Speed = 50;
		}

		namespace lbyBreak
		{
			inline bool Enabled = false;
			inline bool notSend = false;
			inline int angle = 90; // max is 180
		}
	}

	namespace Resolver
	{
		inline bool resolveAll = false;
	}

	namespace ESP
	{
		inline bool enabled = false;
		inline DrawingBackend backend = DrawingBackend::IMGUI;
		inline Filter filter = Filter::Enemies;
		inline ButtonCode_t key = ButtonCode_t::KEY_Z;
		inline TeamColorType teamColorType = TeamColorType::RELATIVE;
		inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
		inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
		inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
		inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
		inline HealthColorVar tColor = ImColor(255, 0, 0, 255);
		inline HealthColorVar tVisibleColor = ImColor(255, 255, 0, 255);
		inline HealthColorVar ctColor = ImColor(0, 0, 255, 255);
		inline HealthColorVar ctVisibleColor = ImColor(0, 255, 0, 255);
		inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
		inline ColorVar bombColor = ImColor(156, 39, 176, 255);
		inline ColorVar bombDefusingColor = ImColor(213, 0, 249, 255);
		inline ColorVar hostageColor = ImColor(121, 85, 72, 255);
		inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
		inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
		inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
		inline ColorVar fishColor = ImColor(255, 255, 255, 255);
		inline ColorVar smokeColor = ImColor(97, 97, 97, 255);
		inline ColorVar decoyColor = ImColor(2255, 152, 0, 255);
		inline ColorVar flashbangColor = ImColor(255, 235, 59, 255);
		inline ColorVar grenadeColor = ImColor(244, 67, 54, 255);
		inline ColorVar molotovColor = ImColor(205, 32, 31, 255);
		inline ColorVar mineColor = ImColor(205, 32, 31, 255);
		inline ColorVar chargeColor = ImColor(205, 32, 31, 255);
		inline ColorVar allyInfoColor = ImColor(255, 255, 255, 255);
		inline ColorVar enemyInfoColor = ImColor(255, 255, 255, 255);

		namespace FilterEnemy
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}
			namespace Glow
			{
				inline bool enabled = false;
				inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
				inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
				inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
				inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
				inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
				inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
				inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
				inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace Skeleton
			{
				inline bool enabled = false;
				inline ColorVar allyColor = ImColor(255, 255, 255, 255);
				inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}

			namespace BulletBeam
			{
				inline bool enabled = false;
			}
			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}

			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace FilterAlice
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}

			namespace Glow
			{
				inline bool enabled = false;
				inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
				inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
				inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
				inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
				inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
				inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
				inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
				inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace Skeleton
			{
				inline bool enabled = false;
				inline ColorVar allyColor = ImColor(255, 255, 255, 255);
				inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}
			namespace BulletBeam
			{
				inline bool enabled = false;
			}
			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}

			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace FilterLocalPlayer
		{
			namespace playerInfo
			{
				inline bool enabled = false;
			}

			namespace Glow
			{
				inline bool enabled = false;
				inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
				inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
				inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
				inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
				inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
				inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
				inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
				inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
			}
			namespace RealChams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace Chams
			{
				inline bool enabled = false;
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
			namespace Skeleton
			{
				inline bool enabled = false;
				inline ColorVar allyColor = ImColor(255, 255, 255, 255);
				inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
			}
			namespace HeadDot
			{
				inline bool enabled = false;
				inline float size = 2.0f;
			}
			namespace BulletTracers
			{
				inline bool enabled = false;
			}

			namespace BulletBeam
			{
				inline bool enabled = false;
			}

			namespace Boxes
			{
				inline bool enabled = false;
				inline BoxType type = BoxType::FRAME_2D;
			}
			namespace HelthBar
			{
				inline bool enabled = false;
				inline BarType type = BarType::VERTICAL;
				inline BarColorType colorType = BarColorType::HEALTH_BASED;
			}

			namespace Tracers
			{
				inline bool enabled = false;
				inline TracerType type = TracerType::BOTTOM;
			}
		}

		namespace Glow
		{
			inline bool enabled = false;
			inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
			inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
			inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
			inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
			inline ColorVar weaponColor = ImColor(158, 158, 158, 255);
			inline ColorVar grenadeColor = ImColor(96, 125, 139, 255);
			inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
			inline ColorVar chickenColor = ImColor(255, 193, 7, 255);
		}

		namespace Filters
		{
			inline bool legit = false;
			inline bool visibilityCheck = false;
			inline bool smokeCheck = false;
			//inline bool flashCheck = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline bool bomb = false;
			inline bool hostages = false;
			inline bool defusers = false;
			inline bool weapons = false;
			inline bool chickens = false;
			inline bool fishes = false;
			inline bool throwables = false;
			inline bool localplayer = false;
		}

		namespace Info
		{
			inline bool name = false;
			inline bool clan = false;
			inline bool steamId = false;
			inline bool rank = false;
			inline bool health = false;
			inline bool armor = false;
			inline bool weapon = false;
			inline bool scoped = false;
			inline bool reloading = false;
			inline bool flashed = false;
			inline bool planting = false;
			inline bool hasDefuser = false;
			inline bool defusing = false;
			inline bool grabbingHostage = false;
			inline bool rescuing = false;
			inline bool location = false;
			inline bool money = false;
		}

		namespace Skeleton
		{
			inline bool enabled = false;
			inline ColorVar allyColor = ImColor(255, 255, 255, 255);
			inline ColorVar enemyColor = ImColor(255, 255, 255, 255);
		}

		namespace Boxes
		{
			inline bool enabled = false;
			inline BoxType type = BoxType::FRAME_2D;
		}

		namespace Sprite
		{
			inline bool enabled = false;
			inline SpriteType type = SpriteType::SPRITE_TUX;
		}

		namespace Bars
		{
			inline bool enabled = false;
			inline BarType type = BarType::HORIZONTAL;
			inline BarColorType colorType = BarColorType::HEALTH_BASED;
		}

		namespace Tracers
		{
			inline bool enabled = false;
			inline TracerType type = TracerType::BOTTOM;
		}

		namespace BulletTracers
		{
			inline bool enabled = false;
		}

		namespace Bomb
		{
			inline bool enabled = false;
		}

		namespace FOVCrosshair
		{
			inline bool enabled = false;
			inline bool filled = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
		}

		namespace Chams
		{

			inline bool enabled = false;
			inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
			inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
			inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
			inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
			inline HealthColorVar localplayerColor = ImColor(0, 255, 255, 255);
			inline HealthColorVar FakeColor = ImColor(124, 145, 25, 225);

			inline ChamsType type = ChamsType::WHITEADDTIVE;

			namespace Arms
			{
				inline bool enabled = false;
				inline ColorVar color = ImColor(255, 255, 255, 255);
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}

			namespace Weapon
			{
				inline bool enabled = false;
				inline ColorVar color = ImColor(255, 255, 255, 255);
				inline ChamsType type = ChamsType::WHITEADDTIVE;
			}
		}

		// sound esp
		namespace Sounds
		{
			inline bool enabled = false;
			inline int time = 1000;
		}

		namespace Hitmarker
		{
			inline bool enabled = false;
			inline bool enemies = false;
			inline bool allies = false;
			inline ColorVar color = ImColor(255, 0, 0, 255);
			inline int duration = 2000;
			inline int size = 16;
			inline int innerGap = 5;

			namespace Damage
			{
				inline bool enabled = false;
			}
			namespace Sounds
			{
				inline bool enabled = false;
				inline Sound sound = Sound::NONE;
			}
		}

		namespace HeadDot
		{
			inline bool enabled = false;
			inline float size = 2.0f;
		}

		namespace Spread
		{
			inline bool enabled = false; // show current spread
			inline bool spreadLimit = false; // show spreadLimit value
			inline ColorVar color = ImColor(15, 200, 45, 255);
			inline ColorVar spreadLimitColor = ImColor(20, 5, 150, 255);
		}

		namespace DangerZone
		{
			inline int drawDist = 2000;
			inline bool drawDistEnabled = false;
			inline bool upgrade = false;
			inline bool lootcrate = false;
			inline bool radarjammer = false;
			inline bool barrel = false;
			inline bool ammobox = false;
			inline bool safe = false;
			inline bool dronegun = false;
			inline bool drone = false;
			inline bool cash = false;
			inline bool tablet = false;
			inline bool healthshot = false;
			inline bool melee = false;
			inline ColorVar upgradeColor = ImColor(255, 0, 0, 255);
			inline ColorVar lootcrateColor = ImColor(255, 0, 0, 255);
			inline ColorVar radarjammerColor = ImColor(255, 0, 0, 255);
			inline ColorVar barrelColor = ImColor(255, 0, 0, 255);
			inline ColorVar ammoboxColor = ImColor(255, 0, 0, 255);
			inline ColorVar safeColor = ImColor(255, 0, 0, 255);
			inline ColorVar dronegunColor = ImColor(255, 0, 0, 255);
			inline ColorVar droneColor = ImColor(255, 0, 0, 255);
			inline ColorVar cashColor = ImColor(255, 0, 0, 255);
			inline ColorVar tabletColor = ImColor(255, 0, 0, 255);
			inline ColorVar healthshotColor = ImColor(255, 0, 0, 255);
			inline ColorVar meleeColor = ImColor(255, 0, 0, 255);
		}
	}

	namespace MaterialConfig
	{
		inline bool enabled = false;
		inline MaterialSystem_Config_t config = {};
	}

	namespace Dlights
	{
		inline bool enabled = false;
		inline float radius = 500.0f;
	}

	namespace Eventlog
	{
		inline bool showEnemies = false;
		inline bool showTeammates = false;
		inline bool showLocalplayer = false;
		inline float duration = 5000;
		inline float lines = 10;
		inline ColorVar color = ImColor(255, 79, 56, 255);
	}

	namespace Spammer
	{
		inline SpammerType type = SpammerType::SPAMMER_NORMAL;
		inline bool say_team = false;

		namespace KillSpammer
		{
			inline bool enabled = false;
			inline bool sayTeam = false;
			inline std::vector<std::string> messages = {};
		}

		namespace RadioSpammer
		{
			inline bool enabled = false;
		}

		namespace NormalSpammer
		{
			inline std::vector<std::string> messages = {};
		}

		namespace PositionSpammer
		{
			inline int team = 1;
			inline bool showName = true;
			inline bool showWeapon = true;
			inline bool showRank = true;
			inline bool showWins = true;
			inline bool showHealth = true;
			inline bool showMoney = true;
			inline bool showLastplace = true;
		}
	}

	namespace BHop
	{
		inline bool enabled = false;

		namespace Chance
		{
			inline bool enabled = false;
			inline int value = 70;
		}

		namespace Hops
		{
			inline bool enabledMax = false;
			inline int Max = 7;
			inline bool enabledMin = false;
			inline int Min = 3;
		}
	}

	namespace NoDuckCooldown
	{
		inline bool enabled = false;
	}

	namespace AutoStrafe
	{
		inline bool enabled = false;
		inline AutostrafeType type = AutostrafeType::AS_FORWARDS;
		inline bool silent = true;
	}

	namespace Noflash
	{
		inline bool enabled = false;
		inline float value = 160.0f; // maxalpha 0-255
	}

	namespace FOVChanger
	{
		inline bool enabled = false;
		inline bool viewmodelEnabled = false;
		inline float value = 100.0f;
		inline float viewmodelValue = 90.0f;
		inline bool ignoreScope = true;
	}

	namespace Radar
	{
		inline bool enabled = false;
		inline float zoom = 16.0f;
		inline bool enemies = false;
		inline bool allies = false;
		inline bool bomb = false;
		inline bool defuser = false;
		inline bool legit = false;
		inline bool visibilityCheck = false;
		inline bool smokeCheck = false;
		inline TeamColorType teamColorType = TeamColorType::RELATIVE;
		inline HealthColorVar enemyColor = ImColor(255, 0, 0, 255);
		inline HealthColorVar enemyVisibleColor = ImColor(255, 255, 0, 255);
		inline HealthColorVar allyColor = ImColor(0, 0, 255, 255);
		inline HealthColorVar allyVisibleColor = ImColor(0, 255, 0, 255);
		inline HealthColorVar tColor = ImColor(255, 0, 0, 255);
		inline HealthColorVar tVisibleColor = ImColor(255, 255, 0, 255);
		inline HealthColorVar ctColor = ImColor(0, 0, 255, 255);
		inline HealthColorVar ctVisibleColor = ImColor(0, 255, 0, 255);
		inline ColorVar bombColor = ImColor(156, 39, 176, 255);
		inline ColorVar bombDefusingColor = ImColor(213, 0, 249, 255);
		inline ColorVar defuserColor = ImColor(49, 27, 146, 255);
		inline float iconsScale = 4.5f;
		inline ImVec2 pos = ImVec2(0, 0);

		namespace InGame
		{
			inline bool enabled = false;
		}
	}

	namespace Recoilcrosshair
	{
		inline bool enabled = false;
		inline bool showOnlyWhenShooting = false;
	}

	namespace RagdollGravity
	{
		inline bool enabled = false;
	}

	namespace Autoblock
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_6;
	}

	namespace SkinOrModel
	{
		inline SkinAndModel skin_or_model = SkinAndModel::Model;
	}

	namespace Skinchanger
	{
		namespace Skins
		{
			inline bool enabled = false;
			inline bool perTeam = false;
		}

		namespace Models
		{
			inline bool enabled = false;
			inline PlayerModel playerModel = PlayerModel::NONE;
		}

		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsCT = {
				{ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/,     {ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ }},
				{ItemDefinitionIndex::WEAPON_KNIFE,                 {ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET,         -1,                       -1,                       -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_CT_SIDE,                {ItemDefinitionIndex::GLOVE_SPECIALIST,                10006,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_T_SIDE,                 {ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND,        10006,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND,     {ItemDefinitionIndex::INVALID,                         10006,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SPORTY,                 {ItemDefinitionIndex::INVALID,                         10018,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SLICK,                  {ItemDefinitionIndex::INVALID,                         10013,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_MOTORCYCLE,             {ItemDefinitionIndex::INVALID,                         10024,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_LEATHER_WRAP,           {ItemDefinitionIndex::INVALID,                         10009,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SPECIALIST,             {ItemDefinitionIndex::INVALID,                         10033,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET,      {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  1337,                     -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_BAYONET,         {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_FLIP,            {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GUT,             {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY,       {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_PUSH,            {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE,  {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_URSUS,           {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_STILETTO,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER,      {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_CSS,             {ItemDefinitionIndex::INVALID,                         -1,                       0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GHOST,           {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				//{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
				{ItemDefinitionIndex::WEAPON_USP_SILENCER,          {ItemDefinitionIndex::INVALID,                         2,                        0.0005f,                  -1,                  -1,                       -1,                          ""}},
		};
		inline std::unordered_map<ItemDefinitionIndex, AttribItem_t, Util::IntHash<ItemDefinitionIndex>> skinsT = {
				{ItemDefinitionIndex::WEAPON_AK47 /*WeaponID*/,     {ItemDefinitionIndex::INVALID /*itemDefinitionIndex*/, 524 /*fallbackPaintKit*/, 0.0005f /*fallbackWear*/, -1 /*fallbackSeed*/, 1337/*fallbackStatTrak*/, -1/*fallbackEntityQuality*/, "TestTux"/*customName*/ }},
				{ItemDefinitionIndex::WEAPON_KNIFE_T,               {ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT,           -1,                       -1,                       -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_T_SIDE,                 {ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND,        10006,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_STUDDED_BLOODHOUND,     {ItemDefinitionIndex::INVALID,                         10006,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SPORTY,                 {ItemDefinitionIndex::INVALID,                         10018,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SLICK,                  {ItemDefinitionIndex::INVALID,                         10013,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_MOTORCYCLE,             {ItemDefinitionIndex::INVALID,                         10024,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_LEATHER_WRAP,           {ItemDefinitionIndex::INVALID,                         10009,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::GLOVE_SPECIALIST,             {ItemDefinitionIndex::INVALID,                         10033,                    0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_M9_BAYONET,      {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_KARAMBIT,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  1337,                     -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_BAYONET,         {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_FLIP,            {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GUT,             {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_BUTTERFLY,       {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_TACTICAL,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_PUSH,            {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_SURVIVAL_BOWIE,  {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_URSUS,           {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GYPSY_JACKKNIFE, {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_STILETTO,        {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_WIDOWMAKER,      {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_CSS,             {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				{ItemDefinitionIndex::WEAPON_KNIFE_GHOST,           {ItemDefinitionIndex::INVALID,                         417,                      0.0005f,                  -1,                  -1,                       -1,                          ""}},
				//{ ItemDefinitionIndex::WEAPON_KNIFEGG, { ItemDefinitionIndex::INVALID, 417, 0.0005f, -1, -1, -1, ""} },
		};
	}

	namespace ShowRanks
	{
		inline bool enabled = false;
	}

	namespace ShowSpectators
	{
		inline bool enabled = false;
	}

	namespace ClanTagChanger
	{
		inline char value[30] = "MissedIT";
		inline bool animation = false;
		inline int animationSpeed = 650;
		inline bool enabled = false;
		inline ClanTagType type = ClanTagType::STATIC;
	}

	namespace View
	{
		namespace NoAimPunch
		{
			inline bool enabled = false;
		}

		namespace NoViewPunch
		{
			inline bool enabled = false;
		}
	}

	namespace FakeLag
	{
		inline bool enabled = false;
		inline bool impulseLag = false;
		inline int value = 9;
		namespace OnShot
		{
			inline bool Enable = false;
			inline int Value = 5;
		}

		namespace AfterShot
		{
			inline bool Enable = false;
			inline int Value = 5;
		}

		namespace InAir
		{
			inline bool Enable = false;
			inline int Value = 5;
		}

	}

	namespace AutoAccept
	{
		inline bool enabled = false;
	}

	namespace NoSky
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(0, 0, 0, 255);
	}

	namespace SkyBox
	{
		inline bool enabled = false;
		inline int skyBoxNumber = 0; // number in skyBoxNames
	}

	namespace ASUSWalls
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(255, 255, 255, 150);
	}

	namespace NoScopeBorder
	{
		inline bool enabled = false;
	}

	namespace SniperCrosshair
	{
		inline bool enabled = false;
	}

	namespace AutoDefuse
	{
		inline bool enabled = false;
		inline bool silent = false;
	}

	namespace NoSmoke
	{
		inline bool enabled = false;
		inline SmokeType type = SmokeType::NONE;
	}

	namespace ScreenshotCleaner
	{
		inline bool enabled = false;
	}

	namespace EdgeJump
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_C;
	}

	namespace NameStealer
	{
		inline bool enabled = false;
		inline int team = 1;
	}

	namespace ThirdPerson
	{
		inline bool enabled = false;
		inline bool toggled = true;
		inline ButtonCode_t toggleThirdPerson = ButtonCode_t::KEY_LALT;
		inline float distance = 100.0f;
	}

	namespace JumpThrow
	{
		inline bool enabled = false;
		inline ButtonCode_t key = ButtonCode_t::KEY_T;
	}

	namespace NoFall
	{
		inline bool enabled = false;
	}

	namespace DisablePostProcessing
	{
		inline bool enabled = false;
	}

	namespace GrenadeHelper
	{
		inline std::vector<GrenadeInfo> grenadeInfos = {};
		inline bool enabled = false;
		inline bool onlyMatchingInfos = true;
		inline bool aimAssist = false;
		inline float aimStep = 5;
		inline float aimDistance = 75;
		inline float aimFov = 45;
		inline ColorVar aimDot = ImColor(10, 10, 200, 255);
		inline ColorVar aimLine = ImColor(200, 200, 200, 255);
		inline ColorVar infoHE = ImColor(7, 183, 7, 255);
		inline ColorVar infoMolotov = ImColor(236, 0, 0, 255);
		inline ColorVar infoSmoke = ImColor(172, 172, 172, 255);
		inline ColorVar infoFlash = ImColor(255, 255, 0, 255);
		inline std::string actMapName = {};
	}

	namespace GrenadePrediction
	{
		inline bool enabled = false;
		inline ColorVar color = ImColor(255, 79, 56, 255);

	}

	namespace TracerEffects
	{
		inline bool enabled = false;
		inline bool serverSide = false;
		inline TracerEffects_t effect = TracerEffects_t::TASER;
		inline int frequency = 1;

	}
	namespace AutoKnife
	{
		inline bool enabled = false;
		inline bool onKey = true;

		namespace Filters
		{
			inline bool enemies = true;
			inline bool allies = false;
		}
	}
	namespace AngleIndicator
	{
		inline bool enabled = false;
	}
	namespace Debug
	{
		namespace AutoWall
		{
			inline bool debugView = false;
		}
		namespace AutoAim
		{
			inline bool drawTarget = false;
			inline Vector target = Vector(0);
		}
		namespace BoneMap
		{
			inline bool draw = false;
			inline bool justDrawDots = false;
			inline int modelID = 1253; // in econItemMap, not itemdefindex
		}
		namespace AnimLayers
		{
			inline bool draw = false;
		}
	}

	void LoadDefaultsOrSave(std::string path);

	void LoadConfig(std::string path);

	void LoadSettings();

	void DeleteConfig(std::string path);

	void SaveGrenadeInfo(std::string path);

	void LoadGrenadeInfo(std::string path);
}
