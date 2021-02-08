#include "../includes.hpp"
#include "interfaces/ibaseclientdll.hpp"
#include "interfaces/interfaces.hpp"
#include <cstdint>
#include <cstring>

uintptr_t getOffset(RecvTable* table, const char* tableName, const char* netvarName) {
    for (int i = 0; i < table->m_nProps; i++) {
        RecvProp prop = table->m_pProps[i];

        if (!strcmp(prop.m_pVarName, netvarName)) {
            return prop.m_Offset;
        }

        if (prop.m_pDataTable) {
            uintptr_t offset = getOffset(prop.m_pDataTable, tableName, netvarName);

            if (offset) {
                return offset + prop.m_Offset;
            }
        }
    }
    return 0;
}

uintptr_t getNetvarOffset(const char* tableName, const char* netvarName) {
    for (ClientClass* cur = Interfaces::client->GetAllClasses(); cur; cur = cur->m_pNext) {
        if (!strcmp(tableName, cur->m_pRecvTable->m_pNetTableName)) {
            return getOffset(cur->m_pRecvTable, tableName, netvarName);
        }
    }
    return 0;
}

bool Netvar::init() {
    Log::log(LOG, "Initialising Netvars...");
    for (auto& nvar: offsets) {
        nvar.second = getNetvarOffset(nvar.first.first.data(), nvar.first.second.data());
        Log::log(LOG, " %s %s | %lx", nvar.first.first.data(), nvar.first.second.data(), nvar.second);
    }

    Log::log(LOG, "Testing netvars by getting m_iHealth from DT_BasePlayer (%lx)", GETNETVAROFFSET("DT_BasePlayer", "m_iHealth"));
    if (GETNETVAROFFSET("DT_BasePlayer", "m_iHealth") != 0x138) {
        Log::log(WARN, "When getting m_iHealth from DT_BasePlayer the value isnt as expected, this could mean there is a problem in getting netvars or the game has just updated.");
    }

    Log::log(LOG, "Initialising offsets");

    Offsets::sendClantag = (Offsets::SendClantag)PatternScan::findFirstInModule("engine_client.so",
            "55 48 89 E5 41 55 49 89 FD 41 54 BF 48 ? ? ? 49 89 F4 53 48 83 EC 08 E8 ? ? ? ? 48 8D 35 ? ? ? ? 48 89 C7 48 89 C3 E8 ? ? ? ? 48 8D 35 ? ? ? ? 4C 89 EA");
    Log::log(LOG, " sendClantag | %lx", Offsets::sendClantag);

    Offsets::setPlayerReady = (Offsets::SetPlayerReady)PatternScan::findFirstInModule("/client_client.so",
            "55 48 89 F7 48 8D 35 ? ? ? ? 48 89 E5 E8 ? ? ? ? 85 C0");
    Log::log(LOG, " setPlayerReady | %lx", Offsets::setPlayerReady);

    Offsets::radarIsHltvCheck = PatternScan::findFirstInModule("/client_client.so", "84 C0 74 50 31 F6");
    Log::log(LOG, " radarIsHltvCheck | %lx", Offsets::radarIsHltvCheck);

    return true;
}