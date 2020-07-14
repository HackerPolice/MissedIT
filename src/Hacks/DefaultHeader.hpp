#pragma once

#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>
#include <vector>
#include <cstdint>
#include <stdio.h>
#include <string>

// SDks
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../SDK/definitions.h"
//Utilities
#include "../Utils/bonemaps.h"
#include "../Utils/entity.h"
#include "../Utils/xorstring.h"
#include "../Utils/math.h"

//Hacks
#include "AimBot/autowall.h"
#include "backtrack.h"
#include "fakelag.h"

#include "../settings.h"
#include "../interfaces.h"
#include "../Hooks/hooks.h"
#include "antiaim.h"
#include "lagcomp.h"