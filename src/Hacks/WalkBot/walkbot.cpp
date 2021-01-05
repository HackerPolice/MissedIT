#include "walkbot.h"

// bool Settings::WalkBot::enabled = false;
// bool Settings::WalkBot::forceReset = false; // if a Reset is queued up by the User
// bool Settings::WalkBot::autobuy = false; // autobuy during FreezeTime
// int Settings::WalkBot::autobuyAt = 0;   // autobuy >= this amount of cash

static unsigned int rally;
static bool reachedEnd;
static bool hasDied;
bool slowLastTick = false;

const std::vector<Vector> tRallyPoints = {Vector(-417.1f, -707.38f, 174.8f), Vector(234.3f, -664.2f, 66.0f),
                                          Vector(-419.8f, -446.0f, 66.0f), Vector(-375.1f, 701.1f, 67.4f)};
const std::vector<Vector> ctRallyPoints = {Vector(262.8f, 2157.6f, -63.2f), Vector(-537.6f, 2110.4f, -58.4f),
                                           Vector(-383.1f, 1489.1f, -60.0f), Vector(-375.1f, 701.1f, 67.4f)};

C_BasePlayer *localPlayer;

void inline Reset()
{
	if (reachedEnd /*|| Settings::WalkBot::forceReset*/ || hasDied) {
		cvar->ConsoleColorPrintf(ColorRGBA(225, 225, 10), XORSTR("--Reset Rally--\n"));
		rally = 0;
		reachedEnd = false;
		// Settings::WalkBot::forceReset = false;
		hasDied = false;
	}
}

bool DoRally(const std::vector<Vector> points, CUserCmd *cmd) // return true if rally is completed.
{
	if (reachedEnd) {
		return true;
	}
	if ((std::abs(localPlayer->GetEyePosition().x - points[rally].x) < 0.6f) &&
	    (std::abs(localPlayer->GetEyePosition().y - points[rally].y) < 0.6f)) {
		if (rally == points.size() - 1) {
			if (!reachedEnd) {
				cvar->ConsoleDPrintf(XORSTR("Reached Rally #%d\n"), rally);
				cvar->ConsoleColorPrintf(ColorRGBA(50, 200, 100), XORSTR("Finished Rally Points!\n"));
				reachedEnd = true;
			}
			return true; // Finished Walking
		}
		cvar->ConsoleDPrintf(XORSTR("Reached Rally #%d\n"), rally);
		rally++;
		cvar->ConsoleDPrintf(XORSTR("Moving To Rally #%d\n"), rally);
	}

	QAngle move = Math::CalcAngle(localPlayer->GetEyePosition(), points[rally]);
	float oldmovementf = 250.0f;
	float oldmovements = 0;
	cmd->buttons |= IN_WALK;
	Math::CorrectMovement(move, cmd, oldmovementf, oldmovements);
	return false;
}

void Walkbot::CreateMove(CUserCmd *cmd)
{
	localPlayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localPlayer || !engine->IsInGame() /*|| Settings::WalkBot::forceReset*/ ) {
		Reset();
		return;
	}

	// Vector src = localPlayer->GetAbsOrigin();
	// QAngle viewAngle = cmd->viewangles;
	// Vector viewVec;
	// Math::AngleVectors(viewAngle, viewVec);
	// trace_t tr;
	// Ray_t ray;
	// float distance;
	// distance = 100;
	// while ( distance < 50 )
	// {
	// 	/* code */ray.Init(src, viewVec);
	// 	trace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, nullptr, &tr);
	// 	distance = tr.endpos.DistTo(src);
	// 	viewAngle.x += 45;
	// 	Math::NormalizeAngles(viewAngle);
	// 	Math::AngleVectors(viewAngle, viewVec);
	// }

	// cmd->forwardmove = 250.f;
}
