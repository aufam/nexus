#ifndef PROJECT_NEXUS_MODBUS_TCP_SERVER_H
#define PROJECT_NEXUS_MODBUS_TCP_SERVER_H

#ifdef __cplusplus
#include "nexus/modbus/api.h"
#include "nexus/tcp/server.h"

namespace Project::nexus::modbus::tcp {

    class Server : public nexus::tcp::Server, public modbus::api::Server {
    public:
        Server();
        virtual ~Server() {}

    protected:
        using nexus::tcp::Server::addCallback;
    };
}

#else
#endif
#endif // PROJECT_NEXUS_MODBUS_TCP_SERVER_H