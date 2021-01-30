#include "aimstep.h"

static QAngle lastAngle = QAngle(0);

void AimStep::Run(QAngle &angle, CUserCmd *cmd, const RageWeapon_t *currentSettings)
{
	if (!currentSettings->aimStepEnabled) {
		return;
	}

	if (!currentSettings->autoShootEnabled) {
		return;
	}

	AimStep::Run(angle, cmd, currentSettings->aimStepMin, currentSettings->aimStepMax);
}

void AimStep::Run(QAngle &angle, CUserCmd *cmd, const LegitWeapon_t *currentSettings)
{
	if (!currentSettings->aimStepEnabled) {
		return;
	}

	if (!currentSettings->autoAimEnabled) {
		return;
	}

	if (currentSettings->smoothEnabled) {
		return;
	}

	AimStep::Run(angle, cmd, currentSettings->aimStepMin, currentSettings->aimStepMax);
}

void AimStep::Run(QAngle &angle, CUserCmd *cmd, float min, float max)
{
	if (!inProgress) {
		lastAngle = cmd->viewangles;
	}

	float fov = fabs(Math::GetFov(lastAngle, angle));

	float randX = Math::float_rand(min, std::min(max, fov));
	float randY = Math::float_rand(min, std::min(max, fov));

	if (!(inProgress = (fov > randX || fov > randY))) {
		return;
	}

	cmd->buttons &= ~(IN_ATTACK); // aimstep in progress, don't shoot.

	QAngle deltaAngle = lastAngle - angle;

	Math::NormalizeAngles(deltaAngle);

	if (deltaAngle.y < 0) {
		lastAngle.y += randY;
	} else {
		lastAngle.y -= randY;
	}

	if (deltaAngle.x < 0) {
		lastAngle.x += randX;
	} else {
		lastAngle.x -= randX;
	}

	angle = lastAngle;
}
