#include "acsafe.h"

void ACSafe::CreateMove(CUserCmd *cmd)
{
	if (!AntiVACKick) {
		return; // TODO detect other stuff like SMAC etc
	}

	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	QAngle deltaAngle = (cmd->viewangles - CreateMove::lastTickViewAngles);
	Math::NormalizeAngles(deltaAngle);

	const float maxDelta = 29.f;

	if (deltaAngle.Normalize() > maxDelta) {
		cmd->viewangles = CreateMove::lastTickViewAngles + deltaAngle * maxDelta;
		Math::NormalizeAngles(cmd->viewangles);
	}
}

void ACSafe::PostPredictionCreateMove(CUserCmd *cmd)
{
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer || !localplayer->IsAlive()) {
		return;
	}

	cmd->viewangles.z = 0;
}

void ACSafe::ShowWarningPopup(WarningType warningType, bool onDisable, bool &acceptedRisk)
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(210, 85));
	if (ImGui::BeginPopupModal(XORSTR("Error###RISKY_FEATURE"))) {

		switch (warningType) {
			case WarningType::WARNING_UNTRUSTED:
				ImGui::Text(XORSTR("%s this feature may get you untrusted!"), onDisable ? "Disabling" : "Enabling");
				break;
			case WarningType::WARNING_VACERROR:
				ImGui::Text(XORSTR("%s this feature may trigger VAC errors!"), onDisable ? "Disabling" : "Enabling");
				break;
		}

		ImGui::Checkbox(XORSTR("I don't care"), &acceptedRisk);

		if (ImGui::Button(XORSTR("OK"))) {
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
}
