#pragma once

#include "../ImGUI/imgui.h"
#include "../SDK/IClientEntity.h"
#include "../SDK/vector.h"
#include "../SDK/IVModelRender.h"
#include "../SDK/IInputSystem.h"

#include <deque>

namespace ESP
{
	extern const char *ranks[];

	struct BulletBream
	{
		std::deque<Vector> bulletPosition;
		int enemyIndex;
	};

	typedef struct BulletBream BulletBream;

	inline BulletBream bulletBeam;

	ImColor GetESPPlayerColor(C_BasePlayer *player, bool visible);

	bool WorldToScreen(const Vector &origin, ImVec2 *const screen);

	//Hooks
	void DrawModelExecute();

	bool PrePaintTraverse(VPANEL vgui_panel, bool force_repaint, bool allow_force);

	void Paint();

	void PaintToUpdateMatrix();

	void CreateMove(CUserCmd *cmd);
}
