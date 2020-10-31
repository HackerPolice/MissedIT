#pragma once

#include "../HeaderFiles.h"

namespace FakeWalk{
    inline bool FakeWalking = true;
    inline int ticks = 0;
    void CreateMove(CUserCmd* cmd);
}