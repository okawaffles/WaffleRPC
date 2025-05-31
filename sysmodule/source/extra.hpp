#pragma once

enum Module {
    Module_WaffleRPC = 9256
};

enum Errors {
    Error_DebugFailure = -1,
    Error_AppInitError = 0,
    Error_ServerInitFail = 1,
    Error_ServerListenFail = 2,
    Error_OptFail = 3,
    Error_AcceptFail = 4,
    Error_SocketConfigInitFail = 5,
};