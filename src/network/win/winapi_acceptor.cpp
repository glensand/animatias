#include "winapi_acceptor.h"

#include "network/win/wsa.h"
#include "network/win/winapi_stream.h"

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <cassert>
#include <string>

namespace io {

    class win_connection final : public win_stream {
        DECLARE_CONSTRUCT_ONLY(win_connection);
    public:
        explicit win_connection(SOCKET socket) {
            m_socket = socket;
            assert(socket != INVALID_SOCKET);
        }

        virtual ~win_connection() override = default;

        virtual bool connect() override {
            assert(false && "connect is not intended to be called on the server's stream");
            return false;
        }

        virtual bool connect(std::string_view ip, std::string_view port) override {
            assert(false && "connect is not intended to be called on the server's stream");
            return false;
        }
    };

    win_acceptor::~win_acceptor() {
        shutdown();
    }

    bool win_acceptor::run() {
        assert(!m_listener_active.load(std::memory_order_acquire));

        initialize_wsa();

        m_listener_socket = INVALID_SOCKET;
        addrinfo* result = nullptr;
        addrinfo hints{ };

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the server address and port
        auto error = getaddrinfo(nullptr, m_port.c_str(), &hints, &result);
        if (error != 0) {
            printf("getaddrinfo failed with error: %d\n", error);
            return false;
        }

        // Create a SOCKET for the server to listen for client connections.
        m_listener_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (m_listener_socket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            return false;
        }

        // Setup the TCP listening socket
        error = bind(m_listener_socket, result->ai_addr, (int)result->ai_addrlen);
        if (error == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            closesocket(m_listener_socket);
            return false;
        }

        freeaddrinfo(result);

        error = ::listen(m_listener_socket, SOMAXCONN);
        if (error == SOCKET_ERROR) {
            printf("listen failed with error: %d\n", WSAGetLastError());
            closesocket(m_listener_socket);
            return false;
        }

        m_listener_active.store(true, std::memory_order_release);
        m_listener_thread = std::thread([this] { listen(); });

        return true;
    }

    void win_acceptor::shutdown() {
        deinitialize_wsa();
        closesocket(m_listener_socket);
        assert(m_listener_active.load(std::memory_order_acquire));
        m_listener_active.store(false, std::memory_order_release);
        assert(m_listener_thread.joinable());
        m_listener_thread.join();
    }

    void win_acceptor::bind_on_new_connection(on_new_connection_t&& callback) {
        m_on_new_connection = std::move(callback);
    }

    void win_acceptor::bind_on_listener_failure(on_listener_failure&& callback) {
        m_on_listener_failure = std::move(callback);
    }

    void win_acceptor::listen() {
        int last_error{ 0 };
        while (last_error == 0) {
            const auto client_socket = accept(m_listener_socket, nullptr, nullptr);
            if (client_socket == INVALID_SOCKET) {
                last_error = WSAGetLastError();
            } else {
                assert(m_on_new_connection);
                m_on_new_connection(new win_connection(client_socket));
            }
        }

        printf("accept failed with error: %d\n", last_error);
        m_listener_active.store(false, std::memory_order_release);
        auto&& error = "accept failed with error: " + std::to_string(last_error);
        if (m_on_listener_failure)
            m_on_listener_failure(error);
    }
}
