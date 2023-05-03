/* Copyright (C) 2023 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/animatias
 */

#pragma once

#include <atomic>
#include <string_view>
#include <thread>
#include <functional>
#include "foundation.h"

namespace io {

    class stream;

    class win_acceptor final {
        DECLARE_CONSTRUCT_ONLY(win_acceptor);

    public:
        win_acceptor() = default;
        virtual ~win_acceptor();

        bool is_active() const noexcept { return m_listener_active.load(std::memory_order_acquire); }
        bool run();
        void shutdown();

        using on_new_connection_t = std::function<void(stream*)>;
        void bind_on_new_connection(on_new_connection_t&& callback);

        using on_listener_failure = std::function<void(const std::string& error)>;
        void bind_on_listener_failure(on_listener_failure&& callback);

    private:
        void listen();

        std::atomic_bool m_listener_active { false };
        std::thread m_listener_thread;
        on_new_connection_t m_on_new_connection;
        on_listener_failure m_on_listener_failure;

        std::string m_port;
        uint64_t m_listener_socket{ 0 };
    };

}
