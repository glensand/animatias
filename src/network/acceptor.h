/* Copyright (C) 2023 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/animatias
 */

#pragma once

#include <functional>
#include <string_view>
#include <string>

#include "foundation.h"

namespace io {

    class stream;

    class acceptor {
        DECLARE_CONSTRUCT_ONLY(acceptor);
    public:
        acceptor() = default;
        virtual ~acceptor() = default;

        virtual bool is_active() const noexcept = 0;
        virtual bool run() = 0;
        virtual void shutdown() = 0;

        using on_new_connection_t = std::function<void(stream*)>;
        virtual void bind_on_new_connection(on_new_connection_t&& callback) = 0;

        using on_listener_failure = std::function<void(const std::string& error)>;
        virtual void bind_on_listener_failure(on_listener_failure&& callback) = 0;
    };

}
