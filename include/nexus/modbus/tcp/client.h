#ifndef PROJECT_NEXUS_MODBUS_RTU_CLIENT_H
#define PROJECT_NEXUS_MODBUS_RTU_CLIENT_H

#ifdef __cplusplus
#include "nexus/abstract/communication.h"
#include "nexus/modbus/api.h"
#include "nexus/tcp/client.h"

namespace Project::nexus::modbus::tcp {

    class Client : public nexus::tcp::Client, public modbus::api::Client, virtual public nexus::abstract::Communication {
    public:
        using nexus::tcp::Client::Client;
        virtual ~Client() {}

        std::string path() const override { return "/modbus_tcp_client"; };
        // std::string post(std::string_view method_name, std::string_view json_request) override;

        std::pair<std::vector<bool>, modbus::Error> ReadCoils(uint16_t register_address, uint16_t n_register);
        std::pair<std::vector<bool>, modbus::Error> ReadDiscreteInputs(uint16_t register_address, uint16_t n_register);
        
        std::pair<std::vector<uint16_t>, modbus::Error> ReadHoldingRegisters(uint16_t register_address, uint16_t n_register);
        std::pair<std::vector<uint16_t>, modbus::Error> ReadInputRegisters(uint16_t register_address, uint16_t n_register);
        
        std::pair<bool, modbus::Error> WriteSingleCoil(uint16_t register_address, bool value);
        std::pair<uint16_t, modbus::Error> WriteSingleRegister(uint16_t register_address, uint16_t value);
        
        std::pair<uint8_t, modbus::Error> ReadExceptionStatus();
        std::pair<std::vector<uint8_t>, modbus::Error> Diagnostic(uint8_t sub_function);

        modbus::Error WriteMultipleCoils(uint16_t register_address, uint16_t n_register, const std::vector<bool>& values);
        modbus::Error WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values);

    protected:
        std::vector<uint8_t> Request(nexus::byte_view buffer) override;

        // Hide
        using nexus::tcp::Client::request;
    };
}

#else
#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_CLIENT_H