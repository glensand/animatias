/* Copyright (C) 2023 Gleb Bezborodov - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the MIT license.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: bezborodoff.gleb@gmail.com, or visit : https://github.com/glensand/animatias
 */

#include "third_party/spsc_queue.h"

#include <thread>
#include <vector>
#include <deque>
#include <string>

namespace io {
    class stream;
    class acceptor;
}

namespace ani {

    class server final {
    public:
        server(io::acceptor* in_acceptor);

    private:
        jt::spsc_queue<io::stream*> m_new_connections;

        struct client_description final {
            std::size_t id{ 0 };
            std::thread uploading_thread;
            jt::spsc_queue<std::string_view> message_queue;
        };

        std::deque<std::string> m_message_storage;
        std::vector<std::thread> m_uploaders;
        io::acceptor* m_acceptor;
    };

}