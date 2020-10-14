#include "hooks.h"
#include <future>

#include "../interfaces.h"
#include "../settings.h"

#include "../Hacks/bhop.h"
#include "../Hacks/noduckcooldown.h"
#include "../Hacks/autostrafe.h"
#include "../Hacks/showranks.h"
#include "../Hacks/autodefuse.h"
#include "../Hacks/jumpthrow.h"
#include "../Hacks/grenadehelper.h"
#include "../Hacks/grenadeprediction.h"
#include "../Hacks/edgejump.h"
#include "../Hacks/autoblock.h"
#include "../Hacks/predictionsystem.h"
#include "../Hacks/AimBot/ragebot.hpp"
#include "../Hacks/AimBot/legitbot.h"
#include "../Hacks/triggerbot.h"
#include "../Hacks/autoknife.h"
#include "../Hacks/AntiAim/antiaim.h"
#include "../Hacks/AntiAim/fakelag.h"
#include "../Hacks/esp.h"
#include "../Hacks/tracereffect.h"
#include "../Hacks/nofall.h"
#include "../Hacks/ragdollgravity.h"
#include "../Hacks/TickManipulation/lagcomp.hpp"
#include "../Hacks/TickManipulation/backtrack.hpp"
#include "../Hacks/AntiAim/fakeduck.h"
#include "../Hacks/AntiAim/fakewalk.hpp"
#include "../Hacks/WalkBot/walkbot.h"
#include "../Hacks/AntiAim/slowwalk.hpp"

QAngle CreateMove::lastTickViewAngles = QAngle(0);

typedef bool (*CreateMoveFn) (void*, float, CUserCmd*);

bool Hooks::CreateMove(void* thisptr, float flInputSampleTime, CUserCmd* cmd)
{
	clientModeVMT->GetOriginalMethod<CreateMoveFn>(25)(thisptr, flInputSampleTime, cmd);

	if (cmd && cmd->command_number)
	{
        // Special thanks to Gre-- I mean Heep ( https://www.unknowncheats.me/forum/counterstrike-global-offensive/290258-updating-bsendpacket-linux.html )
        uintptr_t* rbp;
		
        asm volatile("mov %%rbp, %0" : "=r" (rbp));
        bool *sendPacket = ((*(bool **)rbp) - (int)24);
        CreateMove::sendPacket = true;

		/* run code that affects movement before prediction */
		if (Settings::BHop::enabled) { auto t = std::async(std::launch::async | std::launch::deferred, BHop::CreateMove, cmd); }
		if (Settings::NoDuckCooldown::enabled) { auto t = std::async(std::launch::async | std::launch::deferred, NoDuckCooldown::CreateMove, cmd); }
		if (Settings::AutoStrafe::enabled) { auto t = std::async(std::launch::async | std::launch::deferred, AutoStrafe::CreateMove, cmd); }
		if (Settings::ShowRanks::enabled) { auto t = std::async(std::launch::async | std::launch::deferred, ShowRanks::CreateMove, cmd); }
		if (Settings::AutoDefuse::enabled || Settings::AutoDefuse::silent) { auto t = std::async(std::launch::async | std::launch::deferred, AutoDefuse::CreateMove, cmd); }
		if (Settings::JumpThrow::enabled) { auto t = std::async(std::launch::async | std::launch::deferred, JumpThrow::CreateMove, cmd); }
		
		GrenadeHelper::CreateMove(cmd);
		if( Settings::GrenadePrediction::enabled ) {  auto t = std::async(std::launch::async | std::launch::deferred, GrenadePrediction::CreateMove, cmd); }
        if( Settings::EdgeJump::enabled ) {  auto t = std::async(std::launch::async | std::launch::deferred, EdgeJump::PrePredictionCreateMove, cmd); }
		if( Settings::Autoblock::enabled ) {  auto t = std::async(std::launch::async | std::launch::deferred, Autoblock::CreateMove, cmd); }
		if ( !Settings::NoFall::enabled ) {  auto t = std::async(std::launch::async | std::launch::deferred, NoFall::PrePredictionCreateMove, cmd);}
		// Walkbot::CreateMove(cmd);

		PredictionSystem::StartPrediction(cmd);
		if (!Settings::FakeLag::enabled) {  auto t = std::async(std::launch::async | std::launch::deferred, FakeLag::CreateMove, cmd); }
		if (Settings::Legitbot::enabled) { Legitbot::CreateMove(cmd); }
		if (Settings::Ragebot::enabled) { Ragebot::CreateMove(cmd); }
		Triggerbot::CreateMove(cmd);
		AutoKnife::CreateMove(cmd);
    	AntiAim::CreateMove(cmd);
		FakeDuck::CreateMove(cmd);
		if (Settings::AntiAim::FakeWalk::enabled) { FakeWalk::CreateMove(cmd); }
		SlowWalk::CreateMove(cmd);
		if (Settings::BackTrack::enabled) { BackTrack::CreateMove(cmd); }
		if (Settings::LagComp::enabled) { LagComp::CreateMove(cmd); }
		
		ESP::CreateMove(cmd);
		TracerEffect::CreateMove(cmd);
		RagdollGravity::CreateMove(cvar);
		PredictionSystem::EndPrediction();
		
		EdgeJump::PostPredictionCreateMove(cmd);
		NoFall::PostPredictionCreateMove(cmd);

        *sendPacket = CreateMove::sendPacket;

        if (CreateMove::sendPacket)
            CreateMove::lastTickViewAngles = cmd->viewangles;
	}

	return false;
}
