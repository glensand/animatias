#include "winapi_stream.h"

#include "network/win/wsa.h"
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cassert>

namespace io {

    win_stream::win_stream(std::string_view ip, std::string_view port)
        : m_socket(INVALID_SOCKET)
        , m_server_ip(ip.data())
        , m_server_port(port.data()) {
        initialize_wsa();
    }

    win_stream::~win_stream() {
        win_stream::close();
        deinitialize_wsa();
    }

    bool win_stream::connect(std::string_view ip, std::string_view port) {
        m_server_ip = ip.data();
        m_server_port = port.data();
        return connect();
    }

    bool win_stream::connect() {
        addrinfo* result = nullptr;
        addrinfo hints{};

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        auto error = getaddrinfo(m_server_ip.c_str(),
            m_server_port.c_str(), &hints, &result);
        if (error != 0) {
            return false;
        }

        // Attempt to connect to an address until one succeeds
        for (const auto* ptr = result; ptr != nullptr; ptr = ptr->ai_next) {

            // Create a SOCKET for connecting to server
            m_socket = socket(ptr->ai_family, ptr->ai_socktype,
                ptr->ai_protocol);
            if (m_socket == INVALID_SOCKET) {
                printf("socket failed with error: %d\n", WSAGetLastError());

                return false;
            }

            // Connect to server.
            error = ::connect(m_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (error != SOCKET_ERROR)
                break;

            closesocket(m_socket);
            m_socket = INVALID_SOCKET;
        }

        freeaddrinfo(result);

        return m_socket != INVALID_SOCKET;
    }

    void win_stream::close() {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }

    bool win_stream::write(const void* data, std::size_t length) {
        assert(m_socket != INVALID_SOCKET);
        return SOCKET_ERROR != send(m_socket, (const char*)data, (int)length, 0);
    }

    bool win_stream::read(void* data, std::size_t length) {
        auto remaining_data = int(length);
        while(remaining_data != 0) {
            const auto bytes_received = recv(m_socket, 
                (char*)data + (length - remaining_data), remaining_data, 0);
            if (bytes_received <= 0)
                break;
            remaining_data -= bytes_received;
        }
        return remaining_data != 0;
    }

}
