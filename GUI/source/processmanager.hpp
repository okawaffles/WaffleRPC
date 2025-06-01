#pragma once
#include <switch.h>

u64 wrpc_sysmodule_id = 0x010000000000CAFE;

namespace ProcessManager
{

// check if the sysmodule is currently running
Result checkSysmoduleRunning(bool *running)
{
    u64 process_id = 0x0;
    Result rc = pmdmntGetProcessId(&process_id, wrpc_sysmodule_id);

    if (R_FAILED(rc) || process_id == 0x0)
    {
        *running = false;
        return 0;
    }

    *running = true;
    return 0;
}


Result toggleSysmoduleProcess(bool *isCurrentlyRunning)
{
    if (*isCurrentlyRunning)
    {
        
    }
    else
    {

    }

    return 0;
}

}