#include "hooks.h"
#include "../interfaces.h"

#include "../Utils/xorstring.h"

typedef bool (*WriteUserCmdDeltFn) (void* thisptr,void* edx, int slot, bf_write *buf, int from, int to, bool isnewcommand );

bool Hooks::WriteUsercmdDeltaToBuffer(void* thisptr,void* edx, int slot, bf_write *buf, int from, int to, bool isnewcommand){

    static auto funcAdd = clientVMT->GetOriginalMethod<WriteUserCmdDeltFn>(23);
    funcAdd(thisptr,edx,slot, buf, from, to, isnewcommand );


    // cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("in delta > from : %d to : %d \n"), from, to);
    if (from != -1)
        return true;

    // cvar->ConsoleColorPrintf(ColorRGBA(255,0,0,255), XORSTR("in delta 2\n"));


    from = -1;


    return true;
}