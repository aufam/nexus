#include "nexus/modbus/api.h"
#include <etl/keywords.h>

using namespace nexus;

fun modbus::api::Client::ReadCoils(uint8_t server_address, uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<bool>, modbus::Error> {    
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_COILS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;

    val res = Request(req);
    try {
        if (res.at(2) == res.size() - 3) 
            return {{res.begin() + 3, res.end()}, Error::NONE};
        else
            return {{}, Error::DATA_FRAME};
    }
    catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::ReadDiscreteInputs(uint8_t server_address, uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<bool>, modbus::Error> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_DISCRETE_INPUTS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = Request(req);
    try {
        if (res.at(2) == res.size() - 3) 
            return {{res.begin() + 3, res.end()}, Error::NONE};
        else
            return {{}, Error::DATA_FRAME};
    }
    catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::ReadHoldingRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<uint16_t>, modbus::Error> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_HOLDING_REGISTERS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = Request(req);
    try {
        if (res.at(2) != res.size() - 3)
            return {{}, Error::DATA_FRAME};

        const int length = res.at(2) / 2;
        const int index_begin = 3;

        std::vector<uint16_t> ret;
        ret.reserve(length);
        for (int i = 0; i < length; ++i)
            ret.push_back(res.at(index_begin + i * 2) << 8 | res.at(index_begin + i * 2 + 1));
        
        return {std::move(ret), Error::NONE};
    }
    catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::ReadInputRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register) -> std::pair<std::vector<uint16_t>, modbus::Error> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_INPUT_REGISTERS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = Request(req);
    try {
        if (res.at(2) != res.size() - 3)
            return {{}, Error::DATA_FRAME};

        const int length = res.at(2) / 2;
        const int index_begin = 3;

        std::vector<uint16_t> ret;
        ret.reserve(length);
        for (int i = 0; i < length; ++i)
            ret.push_back(res.at(index_begin + i * 2) << 8 | res.at(index_begin + i * 2 + 1));
        
        return {std::move(ret), Error::NONE};
    }
    catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::WriteSingleCoil(uint8_t server_address, uint16_t register_address, bool value) -> std::pair<bool, Error> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = WRITE_SINGLE_COIL;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = value ? 0xFF : 0x00;
    req[5] = 0x00;
    
    val res = Request(req);
    try {
        if (res.size() < 4 or not std::equal(res.begin(), res.begin() + 4, req))
            return {{}, Error::DATA_FRAME};
        
        if (res.at(4) == 0xFF) {
            return {true, Error::NONE};
        }
        if (res.at(4) == 0x00) {
            return {false, Error::NONE};
        }
        return {{}, Error::DATA_FRAME};
    } catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::WriteSingleRegister(uint8_t server_address, uint16_t register_address, uint16_t value) -> std::pair<uint16_t, Error> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = WRITE_SINGLE_REGISTER;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (value >> 8) & 0xFF;
    req[5] = (value >> 0) & 0xFF;

    val res = Request(req);
    try {
        if (res.size() < 4 or not std::equal(res.begin(), res.begin() + 4, req))
            return {{}, Error::DATA_FRAME};
        
        return {res.at(4) << 8 | res.at(5), Error::NONE};
    } catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::ReadExceptionStatus(uint8_t server_address) -> std::pair<uint8_t, Error> { 
    uint8_t req[2] = {};
    req[0] = server_address;
    req[1] = READ_EXCEPTION_STATUS;

    val res = Request(req);
    try {
        return {res.at(3), Error::NONE};
    } catch (const std::out_of_range&) {
        return {{}, Error::TIMEOUT};
    }
}

fun modbus::api::Client::Diagnostic(uint8_t server_address, uint8_t sub_function) -> std::pair<std::vector<uint8_t>, Error> {
    uint8_t req[3] = {};
    req[0] = server_address;
    req[1] = DIAGNOSTIC;
    req[2] = sub_function;

    val res = Request(req);
    if (res.size() < 2)
        return {{}, Error::TIMEOUT};
    
    return {{res.begin() + 2, res.end()}, Error::NONE};
}

fun modbus::api::Client::WriteMultipleCoils(uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<bool>& values) -> Error {
    int byte_count = values.size() / 8 + 1;
    var req = std::vector<uint8_t>();
    req.reserve(7 + byte_count);
    req.push_back(server_address);
    req.push_back(WRITE_MULTIPLE_COILS);
    req.push_back((register_address >> 8) & 0xFF);
    req.push_back((register_address >> 0) & 0xFF);
    req.push_back((n_register >> 8) & 0xFF);
    req.push_back((n_register >> 0) & 0xFF);
    req.push_back(byte_count);

    uint8_t byte = 0;
    int cnt = 0;
    for (val bit in values) {
        byte |= bit << cnt++;
        if (cnt == 8) {
            req.push_back(byte);
            cnt = 0;
        }
    }
    if (cnt > 0 && cnt < 8) {
        req.push_back(byte);
    }
    
    val res = Request(req);
    if (res.size() < 6)
        return Error::TIMEOUT;
    
    if (not std::equal(res.begin(), res.begin() + 6, req.begin()))
        return Error::DATA_FRAME;
    
    return Error::NONE;
}

fun modbus::api::Client::WriteMultipleRegisters(uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values) -> Error {
    int byte_count = values.size() * 2;
    var req = std::vector<uint8_t>();
    req.reserve(7 + byte_count);
    req.push_back(server_address);
    req.push_back(WRITE_MULTIPLE_REGISTERS);
    req.push_back((register_address >> 8) & 0xFF);
    req.push_back((register_address >> 0) & 0xFF);
    req.push_back((n_register >> 8) & 0xFF);
    req.push_back((n_register >> 0) & 0xFF);
    req.push_back(byte_count);

    for (val reg in values) {
        req.push_back((reg >> 8) & 0xFF);
        req.push_back((reg >> 0) & 0xFF);
    }

    val res = Request(req);
    if (res.size() < 6)
        return Error::TIMEOUT;
    
    if (not std::equal(res.begin(), res.begin() + 6, req.begin()))
        return Error::DATA_FRAME;
    
    return Error::NONE;
}