#ifndef PROJECT_NEXUS_MODBUS_RTU_CLIENT_H
#define PROJECT_NEXUS_MODBUS_RTU_CLIENT_H

#include "nexus/modbus/api_client.h"
#include "nexus/serial/hardware.h"

#ifdef __cplusplus

namespace Project::nexus::modbus::rtu {

    class Client : public modbus::api::Client, public serial::Hardware::Interface {
    public:
        struct Args { 
            int server_address;
            std::string port; 
            speed_t speed;
            std::chrono::milliseconds timeout=std::chrono::milliseconds(100); 
        };

        Client(int server_address, std::string port, speed_t speed, std::chrono::milliseconds timeout=std::chrono::milliseconds(100))
            : Client(Args{server_address, port, speed, timeout}) {}
        
        Client(int server_address, std::shared_ptr<serial::Hardware> ser);
        explicit Client(Args args);
        virtual ~Client();

        std::string path() const override { return "/modbus_rtu_client"; }

        void reconnect() override { ser_->reconnect(); }
        void disconnect() override { ser_->disconnect();  }
        bool isConnected() const override { return ser_->isConnected(); }

        nexus::byte_view request(nexus::byte_view buffer) override;
    };
}

#else
typedef void* nexus_modbus_rtu_client_t;

nexus_modbus_rtu_client_t nexus_modbus_rtu_client_new(int server_address, const char* port, speed_t speed, int timeout);
#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_CLIENT_H