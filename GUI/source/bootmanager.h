#pragma once
#include <switch.h>
#include <dirent.h>
#include <cstring>
#include <fstream>

namespace BootManager
{
    
// check if boot2.flag exists for the
// sysmodule, indicating start on boot
Result checkBoot2Flag(bool *start);

// add or remove boot2.flag from the
// flags directory for the sysmodule
Result toggleBoot2Flag(bool *boot2);

}