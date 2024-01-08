#include "nexus/modbus/tcp/client.h"
#include <etl/keywords.h>

using namespace nexus;

fun modbus::tcp::Client::Request(nexus::byte_view buffer) -> std::vector<uint8_t> {
    var [res, err] = request(modbus::api::encode(buffer));
    if (err != Client::Error::NONE)
        return {};
    
    return modbus::api::decode(res);
}


fun modbus::tcp::Client::ReadCoils(uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<bool>, modbus::Error> {    
    return modbus::api::Client::ReadCoils(0xFF, register_address, n_register);
}

fun modbus::tcp::Client::ReadDiscreteInputs(uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<bool>, modbus::Error> {
    return modbus::api::Client::ReadDiscreteInputs(0xFF, register_address, n_register);
}

fun modbus::tcp::Client::ReadHoldingRegisters(uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<uint16_t>, modbus::Error> {
    return modbus::api::Client::ReadHoldingRegisters(0xFF, register_address, n_register);
}

fun modbus::tcp::Client::ReadInputRegisters(uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<uint16_t>, modbus::Error> {
    return modbus::api::Client::ReadInputRegisters(0xFF, register_address, n_register);
}

fun modbus::tcp::Client::WriteSingleCoil(uint16_t register_address, bool value) -> std::pair<bool, modbus::Error> {
    return modbus::api::Client::WriteSingleCoil(0xFF, register_address, value);
}

fun modbus::tcp::Client::WriteSingleRegister(uint16_t register_address, uint16_t value) -> std::pair<uint16_t, modbus::Error> {
    return modbus::api::Client::WriteSingleRegister(0xFF, register_address, value);
}

fun modbus::tcp::Client::ReadExceptionStatus() -> std::pair<uint8_t, modbus::Error> {    
    return modbus::api::Client::ReadExceptionStatus(0xFF);
}

fun modbus::tcp::Client::Diagnostic(uint8_t sub_function) -> std::pair<std::vector<uint8_t>, modbus::Error> {
    return modbus::api::Client::Diagnostic(0xFF, sub_function);
}

fun modbus::tcp::Client::WriteMultipleCoils(uint16_t register_address, uint16_t n_register, const std::vector<bool>& values) -> modbus::Error {
    return modbus::api::Client::WriteMultipleCoils(0xFF, register_address, n_register, values);
}

fun modbus::tcp::Client::WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values) -> modbus::Error {
    return modbus::api::Client::WriteMultipleRegisters(0xFF, register_address, n_register, values);
}