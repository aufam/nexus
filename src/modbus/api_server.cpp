#include "nexus/modbus/api.h"
#include <etl/keywords.h>

using namespace nexus;

fun modbus::api::Server::CoilGetter(uint16_t register_address, std::function<bool()> getter) -> Server& {
    coil_getters[register_address] = std::move(getter);
    return *this;
}

fun modbus::api::Server::CoilSetter(uint16_t register_address, std::function<void(bool)> setter) -> Server& {
    coil_setters[register_address] = std::move(setter);
    return *this;
}

fun modbus::api::Server::HoldingRegisterGetter(uint16_t register_address, std::function<uint16_t()> getter) -> Server& {
    holding_register_getters[register_address] = std::move(getter);
    return *this;
}

fun modbus::api::Server::HoldingRegisterSetter(uint16_t register_address, std::function<void(uint16_t)> setter) -> Server& {
    holding_register_setters[register_address] = std::move(setter);
    return *this;
}

fun modbus::api::Server::DiscreteInputGetter(uint16_t register_address, std::function<bool()> getter) -> Server& {
    discrete_input_getters[register_address] = std::move(getter);
    return *this;
}

fun modbus::api::Server::AnalogInputGetter(uint16_t register_address, std::function<uint16_t()> getter) -> Server& {
    analog_input_getters[register_address]= std::move(getter);
    return *this;
}

fun modbus::api::Server::ExceptionStatusGetter(std::function<uint8_t()> getter) -> Server& {
    exception_status_getter = std::move(getter);
    return *this;
}

fun modbus::api::Server::DiagnosticGetter(uint8_t sub_function, std::function<std::vector<uint8_t>()> getter) -> Server& {
    diagnostic_getters[sub_function] = std::move(getter);
    return *this;
}

fun modbus::api::Server::callback(uint8_t server_address, nexus::byte_view buffer) const -> std::vector<uint8_t> {
    if (buffer.len() < 2)
        return {};
    
    if (buffer[0] != server_address)
        return {};
    
    switch (buffer[1]) {
        case FunctionCode::READ_COILS: 
            return read_coils_callback(buffer); 
            break;
        case FunctionCode::READ_DISCRETE_INPUTS: 
            return read_discrete_inputs_callback(buffer); 
            break;
        case FunctionCode::READ_HOLDING_REGISTERS: 
            return read_holding_registers_callback(buffer); 
            break;
        case FunctionCode::READ_INPUT_REGISTERS: 
            return read_input_registers_callback(buffer); 
            break;
        case FunctionCode::WRITE_SINGLE_COIL: 
            return write_single_coil_callback(buffer); 
            break;
        case FunctionCode::WRITE_SINGLE_REGISTER: 
            return write_single_register_callback(buffer); 
            break;
        case FunctionCode::READ_EXCEPTION_STATUS: 
            return read_exception_status_callback(buffer); 
            break;
        case FunctionCode::DIAGNOSTIC: 
            return diagnostic_callback(buffer); 
            break;
        case FunctionCode::WRITE_MULTIPLE_COILS: 
            return write_multiple_coils_callback(buffer); 
            break;
        case FunctionCode::WRITE_MULTIPLE_REGISTERS: 
            return write_multiple_registers_callback(buffer); 
            break;
        
        default: break;
    }

    return {};
}

fun modbus::api::Server::read_coils_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], coil_getters.size());

        var res = std::vector<uint8_t>();
        res.reserve(length + 3);
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        res.push_back(length);

        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            res.push_back(coil_getters.at(reg)());
        }

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::read_discrete_inputs_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], discrete_input_getters.size());

        var res = std::vector<uint8_t>();
        res.reserve(length + 3);
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        res.push_back(length);

        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            res.push_back(discrete_input_getters.at(reg)());
        }

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::read_holding_registers_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], holding_register_getters.size());

        var res = std::vector<uint8_t>();
        res.reserve(length * 2 + 3);
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        res.push_back(length * 2);

        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            val value = holding_register_getters.at(reg)();
            res.push_back((value >> 8) & 0xFF);
            res.push_back((value >> 0) & 0xFF);
        }

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::read_input_registers_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], analog_input_getters.size());

        var res = std::vector<uint8_t>();
        res.reserve(length * 2 + 3);
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        res.push_back(length * 2);

        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            val value = analog_input_getters.at(reg)();
            res.push_back((value >> 8) & 0xFF);
            res.push_back((value >> 0) & 0xFF);
        }

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::write_single_coil_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t value = buffer[4] << 8 | buffer[5];
        if (value == 0xFF00) {            
            coil_setters.at(start_register)(true);
            return {buffer.begin(), buffer.end()};
        }
        if (value == 0x0000) {
            coil_setters.at(start_register)(false);
            return {buffer.begin(), buffer.end()};
        }
        return {};
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::write_single_register_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t value = buffer[4] << 8 | buffer[5];
        holding_register_setters.at(start_register)(value);
        return {buffer.begin(), buffer.end()};
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::read_exception_status_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        var res = std::vector<uint8_t>(3);
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        res.push_back(exception_status_getter());
        return res;
    } catch (const std::out_of_range&) {
        return {};
    } catch (const std::bad_function_call&) {
        return {};
    }
}

fun modbus::api::Server::diagnostic_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        val sub_function = buffer[2];
        val diagnostic = diagnostic_getters.at(sub_function)();

        var res = std::vector<uint8_t>();
        res.reserve(2 + diagnostic.size());
        res.push_back(buffer[0]);
        res.push_back(buffer[1]);
        for (val byte in diagnostic)
            res.push_back(byte);
        
        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::write_multiple_coils_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], coil_setters.size());

        const size_t byte_count = buffer[6];
        if (byte_count != buffer.len() - 7)
            return {};
        
        int bit_index = 0;
        var buf_index = 7;
        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            coil_setters.at(reg)((1 << bit_index++) & buffer[buf_index]);
            if (bit_index == 8) {
                bit_index = 0;
                ++buf_index;
            }
        }

        var res = std::vector<uint8_t>(6);
        for (var i in etl::range(6)) 
            res[i] = buffer[i];

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}

fun modbus::api::Server::write_multiple_registers_callback(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    try {
        const uint16_t start_register = buffer[2] << 8 | buffer[3];
        const uint16_t length = etl::min(buffer[4] << 8 | buffer[5], holding_register_setters.size());

        const size_t byte_count = buffer[6];
        if (byte_count != buffer.len() - 7)
            return {};
        
        var buf_index = 7;
        for (val reg in etl::range<uint16_t>(start_register, start_register + length)) {
            holding_register_setters.at(reg)(buffer[buf_index] << 8 | buffer[buf_index + 1]);
            buf_index += 2;
        }

        var res = std::vector<uint8_t>(6);
        for (var i in etl::range(6)) 
            res[i] = buffer[i];

        return res;
    } catch (const std::out_of_range&) {
        return {};
    }
}