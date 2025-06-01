#pragma once
#include <switch.h>
#include <dirent.h>
#include <cstring>
#include <fstream>

namespace BootManager
{
    
// check if boot2.flag exists for the
// sysmodule, indicating start on boot
Result checkBoot2Flag(bool *start)
{
    DIR* dir;
    struct dirent* ent;

    // check if main sysmodule dir exists
    dir = opendir("/atmosphere/contents/010000000000CAFE");
    if (dir == NULL) return 1; // sysmodule not installed

    *start = false;

    dir = opendir("/atmosphere/contents/010000000000CAFE/flags");
    if (dir == NULL)
    {
        // sysmodule installed but no flags dir, 
        // try and create it
        Result rc = mkdir("/atmosphere/contents/010000000000CAFE/flags", 0);
        if (R_SUCCEEDED(rc)) return 0; // bool was set to false and everything succeeded
        else return 2; // bool was set to false but we did not succeed in creating the flags dir
    }

    // dir exists, check if flag exists
    while ((ent = readdir(dir)))
    {
        if (strcmp(ent->d_name, "boot2.flag") == 0) *start = true;
    }
        
    // all succeeded, bool will be set to true if flag was found during the readdir loop
    closedir(dir);
    return 0;
}


Result toggleBoot2Flag(bool *boot2)
{
    Result rc;

    if (*boot2)
    {
        // delete boot2.flag
        rc = remove("sdmc:/atmosphere/contents/010000000000CAFE/flags/boot2.flag");
        
        if (rc == 0)
        {
            return 0;
            *boot2 = false;
        }
        else return 1;
    }
    else
    {
        // create boot2.flag
        std::ofstream file;
        file.open("sdmc:/atmosphere/contents/010000000000CAFE/flags/boot2.flag");
        if (file.is_open()) {
            file.close();
            *boot2 = true;
            return 0;
        }
        else return 2;
    }
}

}