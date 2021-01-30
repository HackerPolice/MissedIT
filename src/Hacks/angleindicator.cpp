#include "angleindicator.h"

#include "../interfaces.h"
#include "../settings.h"
#include "AntiAim/antiaim.h"
#include "../Utils/draw.h"

void AngleIndicator::Paint()
{
	if (!Settings::AntiAim::ShowReal || !engine->IsInGame()) {
		return;
	}

	C_BasePlayer *localPlayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localPlayer || !localPlayer->IsAlive()) {
		return;
	}

	if (Settings::ThirdPerson::toggled) {
		return;
	}

	int x = Paint::engineWidth / 2;
	int y = Paint::engineHeight / 2;

	if (AntiAim::ManualAntiAim::alignBack) {
		Draw::DrawTriangle(Vector2D(x, y + 30), Vector2D(x + 10, y + 10), Vector2D(x - 10, y + 10),
		                   ImColor(255, 0, 0, 255));
	} else if (AntiAim::ManualAntiAim::alignRight) {
		Draw::DrawTriangle(Vector2D(x + 30, y), Vector2D(x + 10, y - 15), Vector2D(x + 10, y + 15),
		                   ImColor(255, 200, 0, 255));
	} else if (AntiAim::ManualAntiAim::alignLeft) {
		Draw::DrawTriangle(Vector2D(x - 30, y), Vector2D(x - 10, y + 15), Vector2D(x - 10, y - 15),
		                   ImColor(255, 200, 0, 255));
	} else if (!Settings::AntiAim::inverted) {
		Draw::DrawTriangle(Vector2D(x - 30, y), Vector2D(x - 10, y + 15), Vector2D(x - 10, y - 15),
		                   ImColor(255, 0, 0, 255));
	} else {
		Draw::DrawTriangle(Vector2D(x + 30, y), Vector2D(x + 10, y - 15), Vector2D(x + 10, y + 15),
		                   ImColor(255, 0, 0, 255));
	}
}
