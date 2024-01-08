#ifndef PROJECT_NEXUS_MODBUS_API_H
#define PROJECT_NEXUS_MODBUS_API_H

#ifdef __cplusplus
#include "nexus/tools/byte_view.h"
#include <functional>
#include <unordered_map>

namespace Project::nexus::modbus {
    enum Error {
        NONE,
        TIMEOUT,
        DATA_FRAME,
    };

    enum FunctionCode {
        READ_COILS = 1,
        READ_DISCRETE_INPUTS = 2,
        READ_HOLDING_REGISTERS = 3,
        READ_INPUT_REGISTERS = 4,
        WRITE_SINGLE_COIL = 5,
        WRITE_SINGLE_REGISTER = 6,
        READ_EXCEPTION_STATUS = 7,
        DIAGNOSTIC = 8,
        WRITE_MULTIPLE_COILS = 15,
        WRITE_MULTIPLE_REGISTERS = 16,
    };
}

namespace Project::nexus::modbus::api {
    std::vector<uint8_t> encode(nexus::byte_view buffer);
    std::vector<uint8_t> decode(nexus::byte_view buffer);
    
    uint16_t crc(nexus::byte_view buffer);

    class Server {
    public:
        Server() = default;
        virtual ~Server() {}

        Server& CoilGetter(uint16_t register_address, std::function<bool()> getter);
        Server& CoilSetter(uint16_t register_address, std::function<void(bool)> setter);

        Server& HoldingRegisterGetter(uint16_t register_address, std::function<uint16_t()> getter);
        Server& HoldingRegisterSetter(uint16_t register_address, std::function<void(uint16_t)> setter);

        Server& DiscreteInputGetter(uint16_t register_address, std::function<bool()> getter);
        Server& AnalogInputGetter(uint16_t register_address, std::function<uint16_t()> getter);
        
        Server& ExceptionStatusGetter(std::function<uint8_t()> getter);
        Server& DiagnosticGetter(uint8_t sub_function, std::function<std::vector<uint8_t>()> getter);
    
    protected:
        virtual std::vector<uint8_t> callback(uint8_t server_address, nexus::byte_view buffer) const;

        virtual std::vector<uint8_t> read_coils_callback(nexus::byte_view buffer) const;
        virtual std::vector<uint8_t> read_discrete_inputs_callback(nexus::byte_view buffer) const;

        virtual std::vector<uint8_t> read_holding_registers_callback(nexus::byte_view buffer) const;
        virtual std::vector<uint8_t> read_input_registers_callback(nexus::byte_view buffer) const;
        
        virtual std::vector<uint8_t> write_single_coil_callback(nexus::byte_view buffer) const;
        virtual std::vector<uint8_t> write_single_register_callback(nexus::byte_view buffer) const;
        
        virtual std::vector<uint8_t> read_exception_status_callback(nexus::byte_view buffer) const;
        virtual std::vector<uint8_t> diagnostic_callback(nexus::byte_view buffer) const;
        
        virtual std::vector<uint8_t> write_multiple_coils_callback(nexus::byte_view buffer) const;
        virtual std::vector<uint8_t> write_multiple_registers_callback(nexus::byte_view buffer) const;

        std::unordered_map<uint16_t, std::function<bool()>> coil_getters;
        std::unordered_map<uint16_t, std::function<void(bool)>> coil_setters;

        std::unordered_map<uint16_t, std::function<uint16_t()>> holding_register_getters;
        std::unordered_map<uint16_t, std::function<void(uint16_t)>> holding_register_setters;

        std::unordered_map<uint16_t, std::function<bool()>> discrete_input_getters;
        std::unordered_map<uint16_t, std::function<uint16_t()>> analog_input_getters;

        std::function<uint8_t()> exception_status_getter;

        std::vector<std::function<std::vector<uint8_t>()>> diagnostic_getters;
    };

    class Client {
    public:
        Client() = default;
        virtual ~Client() {}

    protected:
        std::pair<std::vector<bool>, Error> ReadCoils(uint8_t server_address, uint16_t register_address, uint16_t n_register);
        std::pair<std::vector<bool>, Error> ReadDiscreteInputs(uint8_t server_address, uint16_t register_address, uint16_t n_register);
        
        std::pair<std::vector<uint16_t>, Error> ReadHoldingRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register);
        std::pair<std::vector<uint16_t>, Error> ReadInputRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register);
        
        std::pair<bool, Error> WriteSingleCoil(uint8_t server_address, uint16_t register_address, bool value);
        std::pair<uint16_t, Error> WriteSingleRegister(uint8_t server_address, uint16_t register_address, uint16_t value);
        
        std::pair<uint8_t, Error> ReadExceptionStatus(uint8_t server_address);
        std::pair<std::vector<uint8_t>, Error> Diagnostic(uint8_t server_address, uint8_t sub_function);

        Error WriteMultipleCoils(uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<bool>& values);
        Error WriteMultipleRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values);
    
        virtual std::vector<uint8_t> Request(nexus::byte_view buffer) = 0;
    };
}

#else
#endif
#endif // PROJECT_NEXUS_MODBUS_API_H