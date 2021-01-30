#include "hooks.h"

#include "../interfaces.h"
#include "../shortcuts.h"
#include "../ATGUI/atgui.h"
#include "../Utils/xorstring.h"
#include "../settings.h"

bool SetKeyCodeState::shouldListen = false;
ButtonCode_t *SetKeyCodeState::keyOutput = nullptr;

typedef void (*SetKeyCodeStateFn)(void *, ButtonCode_t, bool);

void Hooks::SetKeyCodeState(void *thisptr, ButtonCode_t code, bool bPressed)
{
	if (SetKeyCodeState::shouldListen && bPressed) {
		SetKeyCodeState::shouldListen = false;
		*SetKeyCodeState::keyOutput = code;
		UI::UpdateWeaponSettings();
	}

	if (!SetKeyCodeState::shouldListen) {
		Shortcuts::SetKeyCodeState(code, bPressed);
	}

	if (code == ButtonCode_t::KEY_PAGEDOWN && bPressed) {
		CEconItemDefinition *item = itemSystem->GetItemDefinitionByIndex(Settings::Debug::BoneMap::modelID);

		for (int i = 1; i < engine->GetMaxClients(); ++i) {
			C_BasePlayer *player = (C_BasePlayer *) entityList->GetClientEntity(i);

			if (!player
			    || player->GetDormant()
			    || !player->IsAlive()
			    || player->GetImmune()) {
				continue;
			}
			cvar->ConsoleDPrintf(XORSTR("Setting player modelindex to %d\n"),
			                     modelInfo->GetModelIndex(item->m_szModel));
			player->SetModelIndex(modelInfo->GetModelIndex(item->m_szModel));
		}
	}

	inputInternalVMT->GetOriginalMethod<SetKeyCodeStateFn>(92)(thisptr, code, bPressed);
}
