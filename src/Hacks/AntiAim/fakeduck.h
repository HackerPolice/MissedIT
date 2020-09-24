#pragma once

#include "../HeaderFiles.h"

namespace FakeDuck
{

//Hooks
void CreateMove(CUserCmd *cmd);
void OverrideView(CViewSetup *pSetup);

}; // namespace FakeDuck