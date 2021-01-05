#include "showranks.h"

struct RankReveal
{
	char _pad[0x10];
	void *ptr;
	int ebx;
	char _pad2[0x300];
};

void ShowRanks::CreateMove(CUserCmd *cmd)
{
	if (!(cmd->buttons & IN_SCORE)) {
		return;
	}

	RankReveal input;
	input.ptr = nullptr;
	input.ebx = 3;

	if (MsgFunc_ServerRankRevealAll) {
		MsgFunc_ServerRankRevealAll((void *) &input);
	}
}
