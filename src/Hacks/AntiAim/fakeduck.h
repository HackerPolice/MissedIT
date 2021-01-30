#pragma once

#include "../HeaderFiles.h"

namespace FakeDuck
{
	inline bool FakeDucking = false;

//Hooks
	void CreateMove(CUserCmd *cmd);

	void OverrideView(CViewSetup *pSetup);

}; // namespace FakeDuck
