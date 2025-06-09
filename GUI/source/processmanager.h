#pragma once
#include <switch.h>

namespace ProcessManager
{

// check if the sysmodule is currently running
Result checkSysmoduleRunning(bool *running);

// toggle the sysmodule immediately (DOES NOT WORK)
Result toggleSysmoduleProcess(bool *isCurrentlyRunning);

}