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
        bool isRunning() const { return server_socket >= 0; }

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
#include <stdint.h>
#include <stddef.h>

typedef void* nexus_tcp_server_t;

nexus_tcp_server_t nexus_tcp_server_new();
void nexus_tcp_server_delete(nexus_tcp_server_t server);

int nexus_tcp_server_listen(nexus_tcp_server_t server, const char* host, int port);
void nexus_tcp_server_stop(nexus_tcp_server_t server);
int nexus_tcp_server_is_running(nexus_tcp_server_t server);

void nexus_tcp_server_add_callback(nexus_tcp_server_t server, uint8_t* (*callback)(const uint8_t* buffer, size_t* length));
void nexus_tcp_server_set_logger(nexus_tcp_server_t server, void (*logger)(const char* ip, const uint8_t* req_buf, size_t req_len, const uint8_t* res_buf, size_t res_len));

#endif
#endif