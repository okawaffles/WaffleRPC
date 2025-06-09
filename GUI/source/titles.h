#pragma once
#include <switch.h>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <jpeglib.h>

namespace Titles
{
    
// dump a game icon to the SD card
Result dumpGameIcon(u64 title_id);

// dump all game icons to the SD card
Result dumpAllGameIcons();

Result getAppControlData(u64 title_id, NsApplicationControlData *appControlData);

}