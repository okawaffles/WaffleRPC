#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <switch/services/ncm.h>
#include "socket.hpp"
#include "extra.hpp"
#include "title.hpp"

#define INNER_HEAP_SIZE 0x80000
#define CURRENT_VERSION ""

#ifdef __cplusplus
extern "C" {
#endif


u32 __nx_applet_type = AppletType::AppletType_None;

void __libnx_initheap(void)
{
    static u8 inner_heap[INNER_HEAP_SIZE];
    extern void* fake_heap_start;
    extern void* fake_heap_end;

    fake_heap_start = inner_heap;
    fake_heap_end   = inner_heap + sizeof(inner_heap);
}

void __appInit(void)
{
    printf("waffle-rpc starting...");
    
    Result rc;

    // open service manager session
    rc = smInitialize();
    if (R_FAILED(rc))
    {
        printf("waffle-rpc failed to initialize service manager!");
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    }

    printf("service manager init success");

    // retrieve current system version (horizon OS)
    rc = setsysInitialize();
    if (R_SUCCEEDED(rc))
    {
        printf("waffle-rpc setsys init success");
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc)) {
            printf("waffle-rpc got horizon os version: %u.%u.%u", fw.major, fw.minor, fw.micro);
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        }
        setsysExit();
    }


    rc = setInitialize();
    if (R_FAILED(rc))
    {
        printf("waffle-rpc set initialize failure");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_AppInitError));
    }
    printf("waffle-rpc set init success");


    rc = pmdmntInitialize();
    if (R_FAILED(rc))
    {
        printf("waffle-rpc pmdmnt initialize failure");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_AppInitError));
    }
    printf("waffle-rpc pmdmnt init success");


    rc = nsInitialize();
    if (R_FAILED(rc))
    {
        printf("waffle-rpc nc initialize failure");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_AppInitError));
    }
    printf("waffle-rpc nc init success");


    rc = pminfoInitialize();
    if (R_FAILED(rc))
    {
        printf("waffle-rpc pminfo initialize failure");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_AppInitError));
    }
    printf("waffle-rpc pminfo init success");


    // setup socket config
    constexpr SocketInitConfig socketConfig = {
        .tcp_tx_buf_size = 0x800,
        .tcp_rx_buf_size = 0x800,
        .tcp_tx_buf_max_size = 0x2EE0,
        .tcp_rx_buf_max_size = 0x2EE0,

        .udp_tx_buf_size = 0x0,
        .udp_rx_buf_size = 0x0,

        .sb_efficiency = 8,

        .num_bsd_sessions = 3,
        .bsd_service_type = BsdServiceType_User,
    };

    rc = socketInitialize(&socketConfig);

    if (R_FAILED(rc))
    {
        printf("waffle-rpc failed to initialize socket config");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_SocketConfigInitFail));
    }


    printf("waffle-rpc __appInit successful!");
    // close service manager session
    smExit();
}

void __appExit(void)
{
    printf("waffle-rpc exiting...");
    closePresenceServer();
    pminfoExit();
    nsExit();
    pmdmntExit();
    setExit();
    printf("waffle-rpc goodbye!");
}


#ifdef __cplusplus
}
#endif


static u64 last_title_id = 0x0;
static u64 last_process_id = 0x0;
static const char* last_game_name = "unknown game";
static u32 last_ip_address = 0x0;

int main(int argc, char* argv[])
{
    Result rc;

    // store our current IP address
    // we'll reset the server when IP address changes
    // some people use wifi in handheld and ethernet when docked, 
    // leading to two separate IP addresses
    nifmGetCurrentIpAddress(&last_ip_address);

    // start the server socket
    printf("waffle-rpc setting up socket...");
    rc = setupPresenceServer();

    if (R_FAILED(rc))
    {
        printf("waffle-rpc socket setup failure!");
        diagAbortWithResult(MAKERESULT(Module_WaffleRPC, Error_ServerInitFail));
    }
    printf("waffle-rpc socket setup success!");

    printf("waffle-rpc starting main loop");

    Result socket_rc;
    u64 process_id;
    u64 title_id;
    u32 ip_address = last_ip_address;

    while (true)
    {   
        if (R_SUCCEEDED(nifmGetCurrentIpAddress(&ip_address)) && ip_address != last_ip_address)
        {
            // restart server to update bind
            closePresenceServer();
            setupPresenceServer();
        }


        // likely could de-nest this a lot
        // will do later once everything is working
        if (R_SUCCEEDED(pmdmntGetApplicationProcessId(&process_id))) 
        {
            if (process_id != last_process_id)
            {
                last_process_id = process_id;

                if (R_SUCCEEDED(pminfoGetProgramId(&title_id, process_id)))
                {
                    if (title_id != last_title_id)
                    {
                        last_title_id = title_id;
                        last_game_name = getNameFromTitleID(title_id);
                    }
                }
            }

            socket_rc = sendPacket(title_id, last_game_name);
        }
        else
        {
            // don't break our connection if there's no data
            printf("waffle-rpc no data to send, home menu?");

#ifndef __PRESENCECLIENT_COMPATIBILITY
            socket_rc = sendPacket(0x000000000000CAFE, "waffle-rpc-nodata");
#else
            socket_rc = sendPacket(0x0, "SNULL");
#endif

        }

        if (socket_rc != 0)
        {
            // if there is an error we can
            // reinitialize the server to
            // hopefully fix said errors
            printf("waffle-rpc socket error (socket_rc < 0), trying to restart presence server");
            closePresenceServer();
            setupPresenceServer();
        }

        // sendPacket(0x010000000000cafe, "waffle-rpc-keepalive");
        svcSleepThread(1e+9L);
    }

    return 0;
}