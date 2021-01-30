#include "hooks.h"
#include "../interfaces.h"

typedef bool (*WriteUserCmdDeltaFn)(void *thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand);

bool Hooks::WriteUsercmdDeltaToBuffer(void *thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand)
{

	static auto funcAdd = clientVMT->GetOriginalMethod<WriteUserCmdDeltaFn>(24);
	funcAdd(thisptr, slot, buf, from, to, isnewcommand);


	// cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("slot : %d, buf address : %d, from : %d, to : %d, \n"),slot, buf, from, to);

	// cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("in delta 2\n"));


	return true;
}

/*



bool __fastcall Hooked_WriteUsercmdDeltaToBuffer(void* ecx, void*, int slot, bf_write* buf, int from, int to, bool isnewcommand)
{
    static auto ofunct = g_pClientHook->GetOriginal <WriteUsercmdDeltaToBufferFn>(24);

    static auto retn = csgo->Utils.FindPatternIDA(GetModuleHandleA(g_Modules[fnv::hash(hs::engine_dll::s().c_str())].c_str()),
        hs::write_user_cmd_retn::s().c_str());

    if ((_ReturnAddress()) != retn || csgo->DoUnload)
        return ofunct(ecx, slot, buf, from, to, isnewcommand);

    if (interfaces.engine->IsConnected() && interfaces.engine->IsInGame()) {
        if (csgo->game_rules->IsFreezeTime())
            return ofunct(ecx, slot, buf, from, to, isnewcommand);


        if (csgo->m_nTickbaseShift <= 0 || csgo->client_state->iChokedCommands > 3)
            return ofunct(ecx, slot, buf, from, to, isnewcommand);

        if (from != -1)
            return true;
        uintptr_t stackbase;
        __asm mov stackbase, ebp;
        CCLCMsg_Move_t* msg = reinterpret_cast<CCLCMsg_Move_t*>(stackbase + 0xFCC);
        auto net_channel = *reinterpret_cast <INetChannel**> (reinterpret_cast <uintptr_t> (csgo->client_state) + 0x9C);
        int32_t new_commands = msg->m_nNewCommands;

        int32_t next_cmdnr = csgo->client_state->nLastOutgoingCommand + csgo->client_state->iChokedCommands + 1;
            int32_t total_new_commands = abs(csgo->m_nTickbaseShift) + new_commands;

        csgo->m_nTickbaseShift -= total_new_commands;

        from = -1;
        msg->m_nNewCommands = total_new_commands;
        msg->m_nBackupCommands = 0;

        for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
            if (!ofunct(ecx, slot, buf, from, to, true))
                return false;

            from = to;
        }

        CUserCmd* last_realCmd = interfaces.input->GetUserCmd(slot, from);
        CUserCmd fromCmd;



        CUserCmd m_nToCmd = *last_realCmd, m_nFromCmd = *last_realCmd;

        CUserCmd toCmd = fromCmd;
        toCmd.command_number++;


        // dont want to tp
        if (vars.ragebot.double_tap_mode == 0)
        toCmd.tick_count += interfaces.global_vars->tickcount + 2 * interfaces.global_vars->tickcount;

        // we want to tp
        if (vars.ragebot.double_tap_mode == 1)
        toCmd.tick_count++;


        for (int i = new_commands; i <= total_new_commands; i++) {
            WriteUsercmd(buf, &toCmd, &fromCmd);
            fromCmd = toCmd;
            fromCmd.viewangles.x = toCmd.viewangles.x;
            fromCmd.impulse = toCmd.impulse;
            fromCmd.weaponselect = toCmd.weaponselect;
            fromCmd.aimdirection.y = toCmd.aimdirection.y;
            fromCmd.weaponsubtype = toCmd.weaponsubtype;
            fromCmd.upmove = toCmd.upmove;
            fromCmd.random_seed = toCmd.random_seed;
            fromCmd.mousedx = toCmd.mousedx;
            fromCmd.pad_0x4C[3] = toCmd.pad_0x4C[3];
            fromCmd.command_number = toCmd.command_number;
            fromCmd.tick_count = toCmd.tick_count;
            fromCmd.mousedy = toCmd.mousedy;
            fromCmd.pad_0x4C[19] = toCmd.pad_0x4C[19];
            fromCmd.hasbeenpredicted = toCmd.hasbeenpredicted;
            fromCmd.pad_0x4C[23] = toCmd.pad_0x4C[23];
            toCmd.command_number++;
            toCmd.tick_count++;

        }
        return true;

    }
    else
        return ofunct(ecx, slot, buf, from, to, isnewcommand);
}


*/
