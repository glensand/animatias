/* Copyright (C) 2023 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/animatias
 */

#pragma once 

#include "network/stream.h"

namespace io {

    class win_stream : public stream {
        DECLARE_CONSTRUCT_ONLY(win_stream);
    public:

        win_stream(std::string_view ip, std::string_view port);
        win_stream() = default;
        virtual ~win_stream() override;

        virtual bool connect(std::string_view ip, std::string_view port) override;
        virtual bool connect() override;
        virtual void close() override;

        virtual bool write(const void* data, std::size_t length) override;
        virtual bool read(void* data, std::size_t length) override;

    protected:
        uint64_t m_socket{ 0 };

    private:
        std::string m_server_ip;
        std::string m_server_port;
    };

}