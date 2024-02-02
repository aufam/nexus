#ifndef PROJECT_NEXUS_MODBUS_API_SERVER_H
#define PROJECT_NEXUS_MODBUS_API_SERVER_H

#include "nexus/modbus/api.h"

#ifdef __cplusplus
#include <functional>
#include <unordered_map>

namespace Project::nexus::modbus::api {
    class Server : virtual public abstract::Restful {
    public:
        Server(int server_address) : server_address(server_address) {}
        virtual ~Server() {}

        std::string json() const override;
        std::string patch(std::string_view json_request) override;

        virtual bool isRunning() const = 0;

        Server& CoilGetter(uint16_t register_address, std::function<bool()> getter);
        Server& CoilSetter(uint16_t register_address, std::function<void(bool)> setter);

        Server& HoldingRegisterGetter(uint16_t register_address, std::function<uint16_t()> getter);
        Server& HoldingRegisterSetter(uint16_t register_address, std::function<void(uint16_t)> setter);

        Server& DiscreteInputGetter(uint16_t register_address, std::function<bool()> getter);
        Server& AnalogInputGetter(uint16_t register_address, std::function<uint16_t()> getter);
        
        Server& ExceptionStatusGetter(std::function<uint8_t()> getter);
        Server& DiagnosticGetter(uint8_t sub_function, std::function<std::vector<uint8_t>()> getter);
    
    protected:
        virtual std::vector<uint8_t> process_callback(nexus::byte_view buffer) const;

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

        int server_address;

        std::unordered_map<uint16_t, std::function<bool()>> coil_getters;
        std::unordered_map<uint16_t, std::function<void(bool)>> coil_setters;

        std::unordered_map<uint16_t, std::function<uint16_t()>> holding_register_getters;
        std::unordered_map<uint16_t, std::function<void(uint16_t)>> holding_register_setters;

        std::unordered_map<uint16_t, std::function<bool()>> discrete_input_getters;
        std::unordered_map<uint16_t, std::function<uint16_t()>> analog_input_getters;

        std::function<uint8_t()> exception_status_getter;

        std::unordered_map<uint8_t, std::function<std::vector<uint8_t>()>> diagnostic_getters;
    };
}

#else

typedef void* nexus_modbus_server_t;

void nexus_modbus_server_delete(nexus_modbus_server_t server);

int nexus_modbus_server_is_running(nexus_modbus_server_t server);

void nexus_modbus_server_coil_getter(nexus_modbus_server_t server, uint16_t register_address, int (*getter)(void* arg), void* arg);
void nexus_modbus_server_coil_setter(nexus_modbus_server_t server, uint16_t register_address, void (*setter)(void* arg, int value), void* arg);

void nexus_modbus_server_holding_register_getter(nexus_modbus_server_t server, uint16_t register_address, uint16_t (*getter)(void* arg), void* arg);
void nexus_modbus_server_holding_register_setter(nexus_modbus_server_t server, uint16_t register_address, void (*setter)(void* arg, uint16_t value), void* arg);

void nexus_modbus_server_discrete_input_getter(nexus_modbus_server_t server, uint16_t register_address, int (*getter)(void* arg), void* arg);
void nexus_modbus_server_analog_input_getter(nexus_modbus_server_t server, uint16_t register_address, uint16_t (*getter)(void* arg), void* arg);

void nexus_modbus_server_exception_status_getter(nexus_modbus_server_t server, uint8_t (*getter)(void* arg), void* arg);
void nexus_modbus_server_diagnostic_getter(nexus_modbus_server_t server, uint8_t sub_function, uint8_t* (*getter)(void* arg, size_t* length), void* arg);

#endif
#endif // PROJECT_NEXUS_MODBUS_API_SERVER_H