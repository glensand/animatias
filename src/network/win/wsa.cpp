#include "network/win/wsa.h"

#include <cstdint>
#include <winsock2.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

static int64_t ActiveClients{ 0 };

namespace io {

    void initialize_wsa() {
        static WSADATA WsaData;
        ++ActiveClients;
        if (ActiveClients == 1) {
            (void)WSAStartup(MAKEWORD(2, 2), &WsaData);
        }
    }

    void deinitialize_wsa() {
        --ActiveClients;
        if (ActiveClients == 0) {
            WSACleanup();
        }
    }
}