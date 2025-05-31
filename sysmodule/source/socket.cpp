#include "socket.hpp"
#include "extra.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h>

static int socket_fd = 0;
static int connection = -1;


// construct and send a packet containing title ID and name
int sendPacket(u64 title_id, const char* name)
{
    // handle if connection is broken
    if (connection < 0)
    {
        printf("waffle-rpc there is no connection!");
        return -1;
    }

    TitlePacket packet;
    // set name and ID
    packet.magic = MAGIC;
    strcpy(packet.name, name);
    packet.title_id = title_id;
    // send packet
    ssize_t sent = send(connection, &packet, sizeof(packet), 0);
    if (sent != sizeof(packet))
    {
        // client may have disconnected
        printf("waffle-rpc client may have disconnected!");
        close(connection);
        connection = -1;
        return -2;
    }

    return 0;
}

Result setupPresenceServer()
{
    // setup socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        // handle socket opt failure
        close(socket_fd);
        return MAKERESULT(Module_WaffleRPC, Error_OptFail);
    }

    const sockaddr_in server_address {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr = {INADDR_ANY},
    };

    // bind to port
    while (bind(socket_fd, reinterpret_cast<const sockaddr *>(&server_address), sizeof(server_address)) == -1)
    {
        // sleep and retry
        printf("waffle-rpc was unable to bind, try again...");
        svcSleepThread(1e+9L);
    }

    listen(socket_fd, 20);

    connection = accept(socket_fd, nullptr, nullptr);
    return 0;
}

void closePresenceServer(void)
{
    close(connection);
    close(socket_fd);
    socketExit();
}