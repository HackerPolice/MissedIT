#pragma once

#include <vector>
#include <cstdint>
#include <thread>
#include "../settings.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/IInputSystem.h"
#include "../SDK/IGameEvent.h"
#include "../interfaces.h"
#include "../Utils/bonemaps.h"
#include "../Utils/entity.h"
#include "../Utils/math.h"
#include "../Utils/xorstring.h"
#include "backtrack.h"
#include "fakelag.h"
#include "../Hooks/hooks.h"
#include "AimBot/autowall.h"
#include "antiaim.h"
#include "lagcomp.h"

#include <thread>
#include <future>
#include <iostream>
#include <stdlib.h>