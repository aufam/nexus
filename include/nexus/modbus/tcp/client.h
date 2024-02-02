#ifndef PROJECT_NEXUS_MODBUS_RTU_CLIENT_H
#define PROJECT_NEXUS_MODBUS_RTU_CLIENT_H

#include "nexus/modbus/api_client.h"
#include "nexus/tcp/client.h"

#ifdef __cplusplus

namespace Project::nexus::modbus::tcp {

    class Client : public modbus::api::Client {
    public:
        struct Args {
            std::string host; 
            int port;
            std::chrono::milliseconds timeout = std::chrono::milliseconds(1000);
        };

        Client(Args args) : modbus::api::Client(0xFF), cli(args.host, args.port, args.timeout) {}
        Client(std::string host, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) 
            : Client(Args{host, port, timeout}) {}
        
        virtual ~Client() {}

        std::string path() const override { return "/modbus_tcp_client"; };

        void reconnect() override { cli.reconnect(); }

        void disconnect() override { cli.disconnect();  }

        bool isConnected() const override { return cli.isConnected(); }

        nexus::byte_view request(nexus::byte_view buffer) override;

    protected:
        nexus::tcp::Client cli;
    };
}

#else
typedef void* nexus_modbus_tcp_client_t;

nexus_modbus_tcp_client_t nexus_modbus_tcp_client_new(const char* host, int port);

#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_CLIENT_H