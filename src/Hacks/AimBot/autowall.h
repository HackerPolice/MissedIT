#pragma once

#include "../DefaultHeader.hpp"

namespace AutoWall
{
	struct FireBulletData
	{
		Vector src;
		trace_t enter_trace;
		Vector direction;
		CTraceFilter filter;
		float trace_length = 0;
		float trace_length_remaining = 0;
		float current_damage = 0;
		int penetrate_count = 0;
	};

	int GetDamage(const Vector& vecPoint, bool teamCheck);
	int GetDamage(const Vector& point, bool teamCheck, FireBulletData& fdata);
	int GetDamage(C_BasePlayer* player, const Vector& point, bool teamCheck, FireBulletData& fdata);
}
