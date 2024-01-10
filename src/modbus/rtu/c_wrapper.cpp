#include "nexus/modbus/rtu/c_wrapper.h"
#include "nexus/modbus/rtu/server.h"
#include "nexus/modbus/rtu/client.h"
#include <etl/keywords.h>

extern "C" {
    fun static cast_server(nexus_modbus_rtu_server_t server) {
        return static_cast<nexus::modbus::rtu::Server*>(server);
    }

    fun static cast_client(nexus_modbus_rtu_client_t client) {
        return static_cast<nexus::modbus::rtu::Client*>(client);
    }

    fun nexus_modbus_rtu_server_new(uint8_t server_address, const char* port, speed_t speed) -> nexus_modbus_rtu_server_t {
        return new nexus::modbus::rtu::Server(nexus::modbus::rtu::Server::Args{.server_address=server_address, .port=port, .speed=speed});
    }

    fun nexus_modbus_rtu_server_delete(nexus_modbus_rtu_server_t server) -> void {
        delete cast_server(server);
    }

    fun nexus_modbus_rtu_server_coil_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, int (*getter)()) -> void {
        cast_server(server)->CoilGetter(register_address, getter);
    }

    fun nexus_modbus_rtu_server_coil_setter(nexus_modbus_rtu_server_t server, uint16_t register_address, void (*setter)(int)) -> void {
        cast_server(server)->CoilSetter(register_address, setter);
    }

    fun nexus_modbus_rtu_server_holding_register_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, uint16_t (*getter)()) -> void {
        cast_server(server)->HoldingRegisterGetter(register_address, getter);
    }

    fun nexus_modbus_rtu_server_holding_register_setter(nexus_modbus_rtu_server_t server, uint16_t register_address, void (*setter)(uint16_t)) -> void {
        cast_server(server)->HoldingRegisterSetter(register_address, setter);
    }

    fun nexus_modbus_rtu_server_discrete_input_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, int (*getter)()) -> void {
        cast_server(server)->DiscreteInputGetter(register_address, getter);
    }

    fun nexus_modbus_rtu_server_analog_input_getter(nexus_modbus_rtu_server_t server, uint16_t register_address, uint16_t (*getter)()) -> void {
        cast_server(server)->AnalogInputGetter(register_address, getter);
    }

    fun nexus_modbus_rtu_server_exception_status_getter(nexus_modbus_rtu_server_t server, uint8_t (*getter)()) -> void {
        cast_server(server)->ExceptionStatusGetter(getter);
    }

    fun nexus_modbus_rtu_server_diagnostic_getter(nexus_modbus_rtu_server_t server, uint8_t sub_function, uint8_t* (*getter)(size_t* length)) -> void {
        cast_server(server)->DiagnosticGetter(sub_function, [getter] () -> std::vector<uint8_t> {
            size_t length;
            var res = getter(&length);
            var ret = std::vector<uint8_t>(res, res + length);
            delete[] res;
            return ret;
        });
    }

    fun nexus_modbus_rtu_client_new(const char* port, speed_t speed) -> nexus_modbus_rtu_client_t {
        return new nexus::modbus::rtu::Client({.port=port, .speed=speed});
    }

    fun nexus_modbus_rtu_client_delete(nexus_modbus_rtu_client_t client) -> void {
        delete cast_client(client);
    }

    fun nexus_modbus_rtu_client_read_coils(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length) -> int* {
        val [res, err] = cast_client(client)->ReadCoils(server_address, register_address, n_register);
        var ret = new int[res.size()];
        for (size_t i = 0; i < res.size(); ++i) {
            ret[i] = res[i];
        }

        *length = res.size();
        return ret;
    }

    fun nexus_modbus_rtu_client_read_discrete_inputs(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length) -> int* {
        val [res, err] = cast_client(client)->ReadDiscreteInputs(server_address, register_address, n_register);
        var ret = new int[res.size()];
        for (size_t i = 0; i < res.size(); ++i) {
            ret[i] = res[i];
        }

        *length = res.size();
        return ret;
    }

    fun nexus_modbus_rtu_client_read_holding_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length) -> uint16_t* {
        val [res, err] = cast_client(client)->ReadHoldingRegisters(server_address, register_address, n_register);
        var ret = new uint16_t[res.size()];
        ::memcpy(ret, res.data(), res.size());
        *length = res.size();
        return ret;
    }

    fun nexus_modbus_rtu_client_read_input_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, size_t* length) -> uint16_t* {
        val [res, err] = cast_client(client)->ReadInputRegisters(server_address, register_address, n_register);
        var ret = new uint16_t[res.size()];
        ::memcpy(ret, res.data(), res.size());
        *length = res.size();
        return ret;
    }

    fun nexus_modbus_rtu_client_write_single_coil(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, int value) -> void {
        cast_client(client)->WriteSingleCoil(server_address, register_address, value);
    }

    fun nexus_modbus_rtu_client_write_single_register(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t value) -> void {
        cast_client(client)->WriteSingleRegister(server_address, register_address, value);
    }

    fun nexus_modbus_rtu_client_read_exception_status(nexus_modbus_rtu_client_t client, uint8_t server_address) -> uint8_t {
        val [res, err] = cast_client(client)->ReadExceptionStatus(server_address);
        return res;
    }

    fun nexus_modbus_rtu_client_diagnostic(nexus_modbus_rtu_client_t client, uint8_t server_address, uint8_t sub_function, size_t* length) -> uint8_t* {
        val [res, err] = cast_client(client)->Diagnostic(server_address, sub_function);
        var ret = new uint8_t[res.size()];
        ::memcpy(ret, res.data(), res.size());
        *length = res.size();
        return ret;
    }

    fun nexus_modbus_rtu_client_write_multiple_coils(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, int* values, size_t length) -> void {
        cast_client(client)->WriteMultipleCoils(server_address, register_address, n_register, std::vector<bool>(values, values + length));
    }

    fun nexus_modbus_rtu_client_write_multiple_registers(nexus_modbus_rtu_client_t client, uint8_t server_address, uint16_t register_address, uint16_t n_register, uint16_t* values, size_t length) -> void {
        cast_client(client)->WriteMultipleRegisters(server_address, register_address, n_register, std::vector<uint16_t>(values, values + length));
    }
}

