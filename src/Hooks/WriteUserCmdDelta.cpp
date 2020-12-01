#include "hooks.h"
#include "../interfaces.h"

#include "../Utils/xorstring.h"

typedef bool (*WriteUserCmdDeltFn) (void* thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand );

bool Hooks::WriteUsercmdDeltaToBuffer(void* thisptr, int slot, bf_write *buf, int from, int to, bool isnewcommand){

    static auto funcAdd = clientVMT->GetOriginalMethod<WriteUserCmdDeltFn>(24);
    funcAdd(thisptr,slot, buf, from, to, isnewcommand );


    // cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("slot : %d, buf address : %d, from : %d, to : %d, \n"),slot, buf, from, to);

    // cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("in delta 2\n"));


    return true;
}