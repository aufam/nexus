#ifndef PROJECT_NEXUS_MODBUS_RTU_CLIENT_H
#define PROJECT_NEXUS_MODBUS_RTU_CLIENT_H

#ifdef __cplusplus
#include "nexus/modbus/api.h"
#include "nexus/serial/serial.h"

namespace Project::nexus::modbus::rtu {

    class Server : public serial::Serial, public modbus::api::Server {
    public:
        /// Arguments for constructing a `Serial` object.
        struct Args { 
            uint8_t server_address; 
            std::string port=Default::port; 
            speed_t speed=Default::speed;
            std::chrono::milliseconds timeout=Default::timeout; 
        };

        /// Constructor with specified arguments.
        explicit Server(Args args);
        Server(uint8_t server_address, std::string port, speed_t speed, std::chrono::milliseconds timeout=Default::timeout) 
            : Server({.server_address=server_address, .port=port, .speed=speed, .timeout=timeout}) {}

        virtual ~Server() {}

        std::string path() const override { return "/modbus_rtu_server"; };
        // std::string post(std::string_view method_name, std::string_view json_request) override;
        
        /// Encode the raw bytes.
        /// @return Encoded message.
        std::vector<uint8_t> encode(nexus::byte_view buffer) const override;

        /// Decode the raw bytes.
        /// @return Decoded message.
        std::vector<uint8_t> decode(nexus::byte_view buffer) const override;
    
    protected:
        uint8_t server_address;
    };
}

#else
#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_CLIENT_H