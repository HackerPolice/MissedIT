#include "HeaderFiles.h"

namespace AimStep
{

	void Run(QAngle &angle, CUserCmd *cmd, const RageWeapon_t *currentSettings);

	void Run(QAngle &angle, CUserCmd *cmd, const LegitWeapon_t *currentSettings);

	void Run(QAngle &angle, CUserCmd *cmd, float min, float max);

	inline bool inProgress = false;
}
