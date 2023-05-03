/* Copyright (C) 2023 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/animatias
 */

#pragma once

#include <string_view>
#include "foundation.h"

namespace io {

    class stream {
        DECLARE_CONSTRUCT_ONLY(stream);
    public:
        stream() = default;
        virtual ~stream() = default;

        virtual bool connect(std::string_view ip, std::string_view port) = 0;
        virtual bool connect() = 0;

        virtual void close() = 0;

        virtual bool write(const void* data, std::size_t length) = 0;
        virtual bool read(void* read, std::size_t length) = 0;

        template<typename T>
        void write(const T& obj) {
            write(&obj, sizeof(obj));
        }

        template<typename T>
        auto read() {
            T to_read;
            read(&to_read, sizeof(to_read));
            return to_read;
        }
    };

}
