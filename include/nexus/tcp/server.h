#ifndef PROJECT_NEXUS_TCP_SERVER_H
#define PROJECT_NEXUS_TCP_SERVER_H

#ifdef __cplusplus
#include <functional>
#include <atomic>
#include "nexus/tools/byte_view.h"

namespace Project::nexus::tcp {

    class Server {
    public:
        Server() {}
        virtual ~Server();
        
        enum Error {
            NONE,
            SOCKET,
            PTON,
            OPT,
            BIND,
            LISTEN,
        };
        
        Error listen(std::string host, int port);

        void stop();

        Server& addCallback(std::function<std::vector<uint8_t>(nexus::byte_view)> callback);
        Server& setLogger(std::function<void(const char*, nexus::byte_view, nexus::byte_view)> logger);

    protected:
        int server_socket = -1;
        std::atomic_bool is_running{false};
        std::vector<std::function<std::vector<uint8_t>(nexus::byte_view)>> callbacks;
        std::function<void(const char*, nexus::byte_view, nexus::byte_view)> logger;
    };
}

#else
#include "nexus/tcp/c_wrapper.h"
#endif
#endif