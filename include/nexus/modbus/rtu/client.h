#ifndef PROJECT_NEXUS_MODBUS_RTU_CLIENT_H
#define PROJECT_NEXUS_MODBUS_RTU_CLIENT_H

#ifdef __cplusplus
#include "nexus/modbus/api.h"
#include "nexus/serial/serial.h"

namespace Project::nexus::modbus::rtu {

    class Client : public serial::Serial, public modbus::api::Client {
    public:
        using serial::Serial::Serial;
        virtual ~Client() {}

        std::string path() const override { return "/modbus_rtu_client"; };
        // std::string post(std::string_view method_name, std::string_view json_request) override;
        
        /// Encode the raw bytes.
        /// @return Encoded message.
        std::vector<uint8_t> encode(nexus::byte_view buffer) const override;

        /// Decode the raw bytes.
        /// @return Decoded message.
        std::vector<uint8_t> decode(nexus::byte_view buffer) const override;

        // expose
        using modbus::api::Client::ReadCoils;
        using modbus::api::Client::ReadDiscreteInputs;
        using modbus::api::Client::ReadHoldingRegisters;
        using modbus::api::Client::ReadInputRegisters;
        using modbus::api::Client::WriteSingleCoil;
        using modbus::api::Client::WriteSingleRegister;
        using modbus::api::Client::ReadExceptionStatus;
        using modbus::api::Client::Diagnostic;
        using modbus::api::Client::WriteMultipleCoils;
        using modbus::api::Client::WriteMultipleRegisters;
        std::vector<uint8_t> Request(nexus::byte_view buffer) override;
        
    protected:
        // Hide
        using serial::Serial::send;
        using serial::Serial::receiveText;
        using serial::Serial::receiveBytes;
        using serial::Serial::addCallback;
    };
}

#else
#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_CLIENT_H