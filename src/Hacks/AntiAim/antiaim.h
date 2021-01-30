#pragma once

#include "../HeaderFiles.h"

namespace AntiAim
{
	inline QAngle realAngle;
	inline QAngle fakeAngle;
	extern QAngle LastTickViewAngle;

	float GetMaxDelta(CCSGOAnimState *animState);

	namespace ManualAntiAim
	{
		inline bool alignBack = false,
				alignRight = false,
				alignLeft = false;
	}

	inline bool sw = false;

	inline bool bSend = true;

	//Hooks
	void CreateMove(CUserCmd *cmd);

	void FrameStageNotify(ClientFrameStage_t stage);

	void OverrideView(CViewSetup *pSetup);
}
