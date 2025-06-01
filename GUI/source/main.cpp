#include <stdlib.h>
#include <switch.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <cstring>

u64 wrpc_sysmodule_id = 0x010000000000CAFE;

int main(int argc, char* argv[])
{
    consoleInit(NULL);
    pmdmntInitialize();

    PadState pad;
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);    
    padInitializeDefault(&pad);

    bool boot2   = false;
    bool running = false;

    // checks if wafflerpc is running
    printf("WaffleRPC: Checking for active process...");
    Result res;
    u64 process_id;
    res = pmdmntGetProcessId(&process_id, wrpc_sysmodule_id);
    if (R_SUCCEEDED(res)) running = true;
    if (process_id == NULL) running = false;
    printf("WaffleRPC is %s.\n", running ? "RUNNING" : "NOT RUNNING");


    // checks for sdmc://atmosphere/contents/010...CAFE/flags/boot2.flag
    printf("WaffleRPC: Checking boot2.flag...");
    consoleUpdate(NULL);

    DIR* dir;
    struct dirent* ent;

    dir = opendir("/atmosphere/contents/010000000000CAFE/flags");
    if (dir == NULL)
    {
        printf("ERR\n");
        boot2 = false;
        
        if (mkdir("/atmosphere/contents/010000000000CAFE/flags", 0) != 0)
        {
            printf("WaffleRPC: Error: directory \"sdmc:/atmosphere/contents/010000000000CAFE/flags\" does not exist.\nDid you install the WaffleRPC sysmodule correctly?\n");
        }
        else
        {
            printf("WaffleRPC: Created directory \"sdmc:/atmosphere/contents/010000000000CAFE/flags\"\n");
            printf("WaffleRPC is set to %s on boot.\n \n", boot2 ? "START" : "NOT START");
        }
    }
    else
    {
        printf("OK\n");

        while ((ent = readdir(dir)))
        {
            // printf("d_name: %s\n", ent->d_name);
            if (strcmp(ent->d_name, "boot2.flag") == 0) boot2 = true;
        }
        closedir(dir);

        printf("WaffleRPC is set to %s on boot.\n \n", boot2 ? "START" : "NOT START");
    }
       
    // printf("Your console must be restarted in order to enable/disable the sysmodule.\nThis behavior may be changed in a later update.\n");
    printf("Press + to exit.\nPress - to reboot to payload.\nPress X to toggle module start on boot.\nPress Y to toggle sysmodule immediately.\n \n");

    consoleUpdate(NULL);

    while (appletMainLoop())
    {
        padUpdate(&pad);
        u32 keys_down = padGetButtonsDown(&pad);

        // exit
        if (keys_down & HidNpadButton_Plus) break;
        // reboot to payload (tested w hekate only)
        if (keys_down & HidNpadButton_Minus) 
        {
            bpcInitialize();
            bpcRebootSystem();
        }

        // toggle sysmodule immediately
        if (keys_down & HidNpadButton_Y)
        {
            Result rc;
            if (running) rc = pmshellTerminateProgram(wrpc_sysmodule_id);
            else rc = pmshellLaunchProgram(0, NULL, &wrpc_sysmodule_id);

            if (R_SUCCEEDED(rc))
            {
                if (running) printf("Stopped WaffleRPC!\n");
                else printf("Started WaffleRPC!\n");

                running = !running;
            }
            else
            {
                if (running) printf("Failed to stop WaffleRPC!\n");
                else printf("Failed to start WaffleRPC!\n");
            }
        }

        // toggling sysmodule on boot
        if (keys_down & HidNpadButton_X)
        {
            // toggle boot2.flag by deleting/creating it
            int rc = 0;

            if (boot2)
            {
                // delete boot2.flag
                rc = remove("sdmc:/atmosphere/contents/010000000000CAFE/flags/boot2.flag");
                
                if (rc == 0)
                {
                    printf("WaffleRPC will no longer start on boot.\n");
                    boot2 = false;
                }
                else 
                {
                    printf("ERROR: Failed to delete boot2.flag!\n");
                }
            }
            else
            {
                // create boot2.flag
                std::ofstream file;
                file.open("sdmc:/atmosphere/contents/010000000000CAFE/flags/boot2.flag");
                if (!file.is_open())
                {
                    printf("ERROR: Failed to create boot2.flag!");
                }
                else
                {
                    printf("WaffleRPC will now start on boot.\n");
                    file.close();
                }
            }
        }

        // keep everything below this line last
        consoleUpdate(NULL);
    }

    pmdmntExit();
    consoleExit(NULL);
    return 0;
}