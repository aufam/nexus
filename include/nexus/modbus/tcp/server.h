#ifndef PROJECT_NEXUS_MODBUS_TCP_SERVER_H
#define PROJECT_NEXUS_MODBUS_TCP_SERVER_H

#include "nexus/modbus/api_server.h"
#include "nexus/tcp/server.h"

#ifdef __cplusplus

namespace Project::nexus::modbus::tcp {

    class Server : virtual public api::Server, virtual public nexus::tcp::Server {
    public:
        Server();
        virtual ~Server() {}

        std::string path() const override { return "/modbus_tcp_server"; };

        bool isRunning() const override { return nexus::tcp::Server::isRunning(); }

    protected:
        using nexus::tcp::Server::addCallback;
    };
}

#else

typedef void* nexus_modbus_tcp_server_t;

nexus_modbus_tcp_server_t nexus_modbus_tcp_server_new();

void nexus_modbus_tcp_server_listen(nexus_modbus_tcp_server_t server, const char* host, int port);
void nexus_modbus_tcp_server_stop(nexus_modbus_tcp_server_t server);
int nexus_modbus_tcp_server_is_running(nexus_modbus_tcp_server_t server);

#endif
#endif // PROJECT_NEXUS_MODBUS_TCP_SERVER_H