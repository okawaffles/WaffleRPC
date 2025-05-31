#pragma once
#include <switch.h>

#define MAGIC 0xFFAADD23
#define PORT  0xCAFE

// presenceclient-py A:\Apps\PresenceClient-Py

#pragma pack(push, 1)
struct TitlePacket {
    u64 magic;
    u64 title_id;
    char name[612];
};
#pragma pack(pop)

int sendPacket(u64 title_id, const char* name);
Result setupPresenceServer();
void closePresenceServer();