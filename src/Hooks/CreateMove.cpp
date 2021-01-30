#include "hooks.h"

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
#include "../Hacks/AimBot/triggerbot.hpp"
#include "../Hacks/autoknife.h"
#include "../Hacks/AntiAim/antiaim.h"
#include "../Hacks/AntiAim/fakelag.h"
#include "../Hacks/esp.h"
#include "../Hacks/tracereffect.h"
#include "../Hacks/nofall.h"
#include "../Hacks/ragdollgravity.h"
#include "../Hacks/TickManipulation/backtrack.hpp"
#include "../Hacks/AntiAim/fakeduck.h"
#include "../Hacks/AntiAim/fakewalk.hpp"
#include "../Hacks/AntiAim/slowwalk.hpp"
#include "../Hacks/AntiAim/fakelag2.hpp"
#include "../Hacks/Visuals/DesyncChams.hpp"
#include "../Hacks/AntiAim/animfix.h"
#include "../Hacks/acsafe.h"

QAngle CreateMove::lastTickViewAngles = QAngle(0);

typedef bool (*CreateMoveFn)(void *, float, CUserCmd *);

bool Hooks::CreateMove(void *thisptr, float flInputSampleTime, CUserCmd *cmd)
{
	static auto funcAdd = clientModeVMT->GetOriginalMethod<CreateMoveFn>(25);
	funcAdd(thisptr, flInputSampleTime, cmd);

	if (cmd && cmd->command_number) {
		// Special thanks to Gre-- I mean Heep ( https://www.unknowncheats.me/forum/counterstrike-global-offensive/290258-updating-bsendpacket-linux.html )
		uintptr_t *rbp;

		asm volatile("mov %%rbp, %0" : "=r" (rbp));
		bool *sendPacket = ((*(bool **) rbp) - (int) 24);

		CreateMove::sendPacket = true;

		// input->
		/* run code that affects movement before prediction */
		if (Settings::BHop::enabled) { BHop::CreateMove(cmd); }
		if (Settings::NoDuckCooldown::enabled) { NoDuckCooldown::CreateMove(cmd); }
		if (Settings::AutoStrafe::enabled) { AutoStrafe::CreateMove(cmd); }
		if (Settings::ShowRanks::enabled) { ShowRanks::CreateMove(cmd); }
		if (Settings::AutoDefuse::enabled || Settings::AutoDefuse::silent) { AutoDefuse::CreateMove(cmd); }
		if (Settings::JumpThrow::enabled) { JumpThrow::CreateMove(cmd); }

		GrenadeHelper::CreateMove(cmd);
		if (Settings::GrenadePrediction::enabled) { GrenadePrediction::CreateMove(cmd); }
		if (Settings::EdgeJump::enabled) { EdgeJump::PrePredictionCreateMove(cmd); }
		if (Settings::Autoblock::enabled) { Autoblock::CreateMove(cmd); }
		if (Settings::NoFall::enabled) { NoFall::PrePredictionCreateMove(cmd); }
		// Walkbot::CreateMove(cmd);

		PredictionSystem::StartPrediction(cmd);
		{
			if (Settings::FakeLag::enabled) { FakeLag::CreateMove(cmd); }

			if (Settings::AntiAim::FakeWalk::enabled) { FakeWalk::CreateMove(cmd); }
			if (Settings::AntiAim::SlowWalk::enabled) { SlowWalk::CreateMove(cmd); }
			if (Settings::AntiAim::FakeDuck::enabled) { FakeDuck::CreateMove(cmd); }

			// Diff backtrack features
			if (Settings::BackTrack::enabled) { BackTrack::CreateMove(cmd); }
			if (Settings::LagComp::enabled) { LagComp::CreateMove(cmd); }

			// Aimbots
			if (Settings::Ragebot::enabled) { Ragebot::CreateMove(cmd); }
			if (Settings::Legitbot::enabled) { Legitbot::CreateMove(cmd); }
			if (Settings::AntiAim::Enabled) { AntiAim::CreateMove(cmd); }

			FakeLag2::CreateMove(cmd);

			DsyncChams::CreateMove(cmd);
			Triggerbot::CreateMove(cmd);
			AutoKnife::CreateMove(cmd);

			// RapidFire::CreateMove(cmd);

			ESP::CreateMove(cmd);
			TracerEffect::CreateMove(cmd);
			RagdollGravity::CreateMove(cvar);

			ACSafe::CreateMove(cmd);
		}

		PredictionSystem::EndPrediction();

		EdgeJump::PostPredictionCreateMove(cmd);
		NoFall::PostPredictionCreateMove(cmd);
		ACSafe::PostPredictionCreateMove(cmd);

		if ((*sendPacket = CreateMove::sendPacket)) {
			AnimFix::animfix = true;
			CreateMove::lastTickViewAngles = cmd->viewangles;
		}
	}

	return false;
}
