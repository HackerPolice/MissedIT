#pragma once

#include "HeaderFiles.h"

enum class WarningType : int
{
	WARNING_UNTRUSTED,
	WARNING_VACERROR
};

namespace ACSafe
{
	//Hooks
	void CreateMove(CUserCmd *cmd);

	void PostPredictionCreateMove(CUserCmd *cmd);

	//Util

	inline bool AntiVACKick = false;

	void ShowWarningPopup(WarningType warningType, bool onDisable, bool &acceptedRisk);
}
