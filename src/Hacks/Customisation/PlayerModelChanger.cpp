#include "PlayerModelChanger.hpp"

#include "../settings.h"
#include "../interfaces.h"

static int originalIdx = 0;
static bool GetIndex = false;

static int modelIndex[static_cast<int>(PlayerModel::NONE)];

// Thanks https://github.com/Franc1sco/Franug-AgentsChooser/blob/master/csgo_agentschooser.sp for all the names and paths
static const char *modelPaths[] = {
		"models/player/custom_player/legacy/ctm_st6_varianti.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantf.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantb.mdl",
		"models/player/custom_player/legacy/ctm_st6_variante.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantk.mdl",
		"models/player/custom_player/legacy/ctm_sas_variantf.mdl",
		"models/player/custom_player/legacy/ctm_swat_variantj.mdl",
		"models/player/custom_player/legacy/ctm_swat_varianth.mdl",
		"models/player/custom_player/legacy/tm_phoenix_variantf.mdl",
		"models/player/custom_player/legacy/tm_phoenix_varianth.mdl",
		"models/player/custom_player/legacy/tm_leet_variantg.mdl",
		"models/player/custom_player/legacy/tm_phoenix_varianti.mdl",
		"models/player/custom_player/legacy/tm_balkan_variantl.mdl",
		"models/player/custom_player/legacy/ctm_fbi_variantg.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantg.mdl",
		"models/player/custom_player/legacy/ctm_swat_variantg.mdl",
		"models/player/custom_player/legacy/ctm_swat_varianti.mdl",
		"models/player/custom_player/legacy/ctm_st6_variantj.mdl"
};

static void GetIndexes(C_BasePlayer *localplayer)
{
	{
		int size = static_cast<int>(PlayerModel::NONE);
		for (int i = 0; i < size; i++) {
			modelIndex[i] = modelInfo->GetModelIndex(modelPaths[i]);
		}
		originalIdx = *localplayer->GetModelIndex();
		GetIndex = true;
	}
}

void PlayerModelChanger::FrameStageNotifyModels(ClientFrameStage_t stage)
{

	if (!Settings::Skinchanger::Models::enabled) {
		return;
	}
	if (!engine->IsInGame()) {
		return;
	}
	C_BasePlayer *localplayer = (C_BasePlayer *) entityList->GetClientEntity(engine->GetLocalPlayer());
	if (!localplayer) {
		return;
	}
	if (!localplayer->IsAlive()) {
		return;
	}
	if (Settings::Skinchanger::Models::playerModel == PlayerModel::NONE) {
		return;
	}

	if (!GetIndex) { GetIndexes(localplayer); }

	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		int idx = modelIndex[static_cast<int>(Settings::Skinchanger::Models::playerModel)];
		localplayer->SetModelIndex(idx);
		if (!localplayer->GetModel()) {
			GetIndexes(localplayer);
		}

	} else if (stage == ClientFrameStage_t::FRAME_RENDER_END) {
		localplayer->SetModelIndex(originalIdx);
	}

}
