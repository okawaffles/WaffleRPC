#include <stdlib.h>
#include <switch.h>
#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <cstring>

#include "bootmanager.hpp"
#include "processmanager.hpp"

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
    ProcessManager::checkSysmoduleRunning(&running);
    printf("OK\nWaffleRPC is %s.\n", running ? "RUNNING" : "NOT RUNNING");


    // checks for sdmc://atmosphere/contents/010...CAFE/flags/boot2.flag
    printf("WaffleRPC: Checking boot2.flag...");
    consoleUpdate(NULL);

    Result success = BootManager::checkBoot2Flag(&boot2);

    if (R_SUCCEEDED(success)) 
    {
        printf("OK\nWaffleRPC is set to %s on boot.\n \n", boot2 ? "START" : "NOT START");
    }
    else
    {
        if (success == 1) printf("ERR\nError: directory \"sdmc:/atmosphere/contents/010000000000CAFE\" does not exist.\nDid you install the WaffleRPC sysmodule correctly?\n");
        if (success == 2) 
        {
            printf("OK\nError: directory \"sdmc:/atmosphere/contents/010000000000CAFE/flags\" does not exist and could not be created.\n");
            printf("WaffleRPC is set to %s on boot.\n \n", boot2 ? "START" : "NOT START");
        }
    }
       
    printf("Your console must be restarted in order to enable/disable the sysmodule.\nThis behavior may be changed in a later update.\n");
    printf("Press + to exit.\nPress - to reboot to payload.\nPress X to toggle module start on boot.\n \n");

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
            // might be a better way to do this but it works for me lol
            bpcInitialize();
            bpcRebootSystem();
        }

        // toggle sysmodule immediately
        // this code doesn't work
        // if (keys_down & HidNpadButton_Y)
        // {
        //     Result rc;
        //     if (running) rc = pmshellTerminateProgram(wrpc_sysmodule_id);
        //     else rc = pmshellLaunchProgram(0, NULL, &wrpc_sysmodule_id);

        //     if (R_SUCCEEDED(rc))
        //     {
        //         if (running) printf("Stopped WaffleRPC!\n");
        //         else printf("Started WaffleRPC!\n");

        //         running = !running;
        //     }
        //     else
        //     {
        //         if (running) printf("Failed to stop WaffleRPC!\n");
        //         else printf("Failed to start WaffleRPC!\n");
        //     }
        // }

        // toggling sysmodule on boot
        if (keys_down & HidNpadButton_X)
        {
            // toggle boot2.flag by deleting/creating it
            Result rc = BootManager::toggleBoot2Flag(&boot2);

            if (R_SUCCEEDED(rc))
            {
                if (boot2) printf("WaffleRPC will now start on boot!\n");
                else printf("WaffleRPC will no longer start on boot.\n");
            }
            else
            {
                if (rc == 1) printf("Error: Failed to delete boot2.flag!\n");
                if (rc == 2) printf("Error: Failed to create boot2.flag!\n");
            }
        }

        // keep everything below this line last
        consoleUpdate(NULL);
    }

    pmdmntExit();
    consoleExit(NULL);
    return 0;
}