#include "nexus/modbus/api_client.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

using namespace nexus;

fun static create_error_json(modbus::Error err) {
    return "\"error\": " + std::string(
        err == modbus::Error::TIMEOUT ? "\"Timeout\"" : 
        err == modbus::Error::DATA_FRAME ? "\"Data Frame\"" : 
        "\"None\""
    );
}

fun modbus::api::Client::json() const -> std::string {
    return tools::json_concat(abstract::Client::json(), "{"
        "\"serverAddress\": " + std::to_string(server_address) + ", " +
        create_error_json(error_) + 
    "}");
}

fun modbus::api::Client::post(std::string_view method_name, std::string_view json_request) -> std::string {
    val json = tools::json_parse(json_request);

    if (method_name == "read_coils" or method_name == "read_discrete_inputs") {
        val register_address = json["register_address"];
        val n_register = json["n_register"];

        if (not register_address.is_number() or not n_register.is_number())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val res = method_name == "read_coils" 
            ? ReadCoils(register_address.to_int(), n_register.to_int()) 
            : ReadDiscreteInputs(register_address.to_int(), n_register.to_int());

        var res_string = std::string("[");
        for (val coil in res)
            res_string += std::string(coil ? "true" : "false") + ", ";
        res_string.pop_back();
        res_string.back() = ']';

        return tools::json_concat(
            tools::json_response_status_success(method_name == "read_coils" ? "Read Coils" : "Read Discrete Inputs"), 
            "{"
                "\"res\": " + res_string + ", " +
                create_error_json(error_) + 
            "}"
        );
    }

    if (method_name == "read_holding_registers" or method_name == "read_input_registers") {
        val register_address = json["register_address"];
        val n_register = json["n_register"];

        if (not register_address.is_number() or not n_register.is_number())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val res = method_name == "read_holding_registers" 
            ? ReadHoldingRegisters(register_address.to_int(), n_register.to_int()) 
            : ReadInputRegisters(register_address.to_int(), n_register.to_int());

        var res_string = std::string("[");
        for (val value in res)
            res_string += std::to_string(value) + ", ";
        res_string.pop_back();
        res_string.back() = ']';

        return tools::json_concat(
            tools::json_response_status_success(method_name == "read_holding_registers" ? "Read Holding Registers" : "Read Input Registers"), 
            "{"
                "\"res\": " + res_string + ", " +
                create_error_json(error_) + 
            "}"
        );
    }

    if (method_name == "write_single_coil") {
        val register_address = json["register_address"];
        val value = json["value"];

        if (not register_address.is_number() or not value.is_bool())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val res = WriteSingleCoil(register_address.to_int(), value.is_true());
        return tools::json_concat(tools::json_response_status_success("Write Single Coil"), "{"
            "\"res\": " + std::string(res ? "true" : "false") + ", " +
            create_error_json(error_) + 
        "}");
    }

    if (method_name == "write_single_register") {
        val register_address = json["register_address"];
        val value = json["value"];

        if (not register_address.is_number() or not value.is_number())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val res = WriteSingleRegister(register_address.to_int(), value.to_int());
        return tools::json_concat(tools::json_response_status_success("Write Single Register"), "{"
            "\"res\": " + std::to_string(res) + ", " + 
            create_error_json(error_) + 
        "}");
    }

    if (method_name == "read_exception_status") {
        val res = ReadExceptionStatus();
        return tools::json_concat(tools::json_response_status_success("Read Exception Status"), "{"
            "\"res\": " + std::to_string(res) + ", " +
            create_error_json(error_) + 
        "}");
    }

    if (method_name == "diagnostic") {
        val res = ReadExceptionStatus();
        return tools::json_concat(tools::json_response_status_success("Read Exception Status"), "{"
            "\"res\": " + std::to_string(res) + ", " +
            create_error_json(error_) + 
        "}");
    }

    if (method_name == "write_multiple_registers") {
        val register_address = json["register_address"];
        val n_register = json["n_register"];
        val values = json["values"];

        if (not register_address.is_number() or not n_register.is_number() or not values.is_list())
            return tools::json_response_status_fail_mismatch_value_type();

        std::vector<uint16_t> req(values.len());
        for (var [src, dest] in etl::zip(values, req))
            dest = src.to_int();
        
        WriteMultipleRegisters(register_address.to_int(), n_register.to_int(), req);

        return tools::json_concat(tools::json_response_status_success("Write Multiple Registers"), "{" +
            create_error_json(error_) + 
        "}");
    }

    if (method_name == "write_multiple_coils") {
        val register_address = json["register_address"];
        val n_register = json["n_register"];
        val values = json["values"];

        if (not register_address.is_number() or not n_register.is_number() or not values.is_list())
            return tools::json_response_status_fail_mismatch_value_type();

        std::vector<bool> req(values.len());
        for (var [src, dest] in etl::zip(values, req))
            dest = src.is_true();
        
        WriteMultipleCoils(register_address.to_int(), n_register.to_int(), req);

        return tools::json_concat(tools::json_response_status_success("Write Multiple Coils"), "{" +
            create_error_json(error_) + 
        "}");
    }

    return abstract::Client::post(method_name, json_request);
}

fun modbus::api::Client::ReadCoils(uint16_t register_address, uint16_t n_register) -> std::vector<bool> {    
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_COILS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;

    val res = request(req);
    val return_ = [this] (std::vector<bool> res, Error err) { error_ = err; return res; };

    try {
        bool byte_count_size_is_2 = false;
        size_t byte_count = res[2];

        if (byte_count != res.size() - 3) {
            byte_count = (res[2] << 8) | res[3];
            if (byte_count == res.size() - 4) {
                byte_count_size_is_2 = true;
            } else {
                return return_({}, Error::DATA_FRAME);
            }
        }

        const int index_begin = byte_count_size_is_2 ? 4 : 3;
        return return_(std::vector<bool>(res.begin() + index_begin, res.end()), Error::NONE);
    }
    catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::ReadDiscreteInputs(uint16_t register_address, uint16_t n_register) -> std::vector<bool> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_DISCRETE_INPUTS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = request(req);
    val return_ = [this] (std::vector<bool> res, Error err) { error_ = err; return res; };

    try {
        bool byte_count_size_is_2 = false;
        size_t byte_count = res[2];

        if (byte_count != res.size() - 3) {
            byte_count = (res[2] << 8) | res[3];
            if (byte_count == res.size() - 4) {
                byte_count_size_is_2 = true;
            } else {
                return return_({}, Error::DATA_FRAME);
            }
        }

        const int index_begin = byte_count_size_is_2 ? 4 : 3;
        return return_(std::vector<bool>(res.begin() + index_begin, res.end()), Error::NONE);
    }
    catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::ReadHoldingRegisters(uint16_t register_address, uint16_t n_register) -> std::vector<uint16_t> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_HOLDING_REGISTERS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = request(req);
    val return_ = [this] (std::vector<uint16_t> res, Error err) { error_ = err; return res; };

    try {
        bool byte_count_size_is_2 = false;
        size_t byte_count = res[2];

        if (byte_count != res.size() - 3) {
            byte_count = (res[2] << 8) | res[3];
            if (byte_count == res.size() - 4) {
                byte_count_size_is_2 = true;
            } else {
                return return_({}, Error::DATA_FRAME);
            }
        }

        const int length = byte_count / 2;
        const int index_begin = byte_count_size_is_2 ? 4 : 3;

        std::vector<uint16_t> ret;
        ret.reserve(length);
        for (int i = 0; i < length; ++i)
            ret.push_back(res[index_begin + i * 2] << 8 | res[index_begin + i * 2 + 1]);
        
        return return_(std::move(ret), Error::NONE);
    }
    catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::ReadInputRegisters(uint16_t register_address, uint16_t n_register) -> std::vector<uint16_t> {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = READ_INPUT_REGISTERS;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (n_register >> 8) & 0xFF;
    req[5] = (n_register >> 0) & 0xFF;
    
    val res = request(req);
    val return_ = [this] (std::vector<uint16_t> res, Error err) { error_ = err; return res; };

    try {
        bool byte_count_size_is_2 = false;
        size_t byte_count = res[2];

        if (byte_count != res.size() - 3) {
            byte_count = (res[2] << 8) | res[3];
            if (byte_count == res.size() - 4) {
                byte_count_size_is_2 = true;
            } else {
                return return_({}, Error::DATA_FRAME);
            }
        }

        const int length = byte_count / 2;
        const int index_begin = byte_count_size_is_2 ? 4 : 3;

        std::vector<uint16_t> ret;
        ret.reserve(length);
        for (int i = 0; i < length; ++i)
            ret.push_back(res[index_begin + i * 2] << 8 | res[index_begin + i * 2 + 1]);
        
        return return_(std::move(ret), Error::NONE);
    }
    catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::WriteSingleCoil(uint16_t register_address, bool value) -> bool {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = WRITE_SINGLE_COIL;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = value ? 0xFF : 0x00;
    req[5] = 0x00;
    
    val res = request(req);
    val return_ = [this] (bool res, Error err) { error_ = err; return std::move(res); };

    try {
        if (res.size() < 4 or not std::equal(res.begin(), res.begin() + 4, req))
            return return_({}, Error::DATA_FRAME);
        
        if (res[4] == 0xFF) 
            return return_(true, Error::NONE);
        
        if (res[4] == 0x00) 
            return return_(false, Error::NONE);

        return return_({}, Error::DATA_FRAME);
    } catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::WriteSingleRegister(uint16_t register_address, uint16_t value) -> uint16_t {
    uint8_t req[6] = {};
    req[0] = server_address;
    req[1] = WRITE_SINGLE_REGISTER;
    req[2] = (register_address >> 8) & 0xFF;
    req[3] = (register_address >> 0) & 0xFF;
    req[4] = (value >> 8) & 0xFF;
    req[5] = (value >> 0) & 0xFF;

    val res = request(req);
    val return_ = [this] (uint16_t res, Error err) { error_ = err; return std::move(res); };

    try {
        if (res.size() < 4 or not std::equal(res.begin(), res.begin() + 4, req))
            return return_({}, Error::DATA_FRAME);
        
        return return_(res[4] << 8 | res[5], Error::NONE);
    } catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::ReadExceptionStatus() -> uint8_t { 
    uint8_t req[2] = {};
    req[0] = server_address;
    req[1] = READ_EXCEPTION_STATUS;

    val res = request(req);
    val return_ = [this] (uint8_t res, Error err) { error_ = err; return std::move(res); };

    try {
        return return_(res[3], Error::NONE);
    } catch (const std::out_of_range&) {
        return return_({}, Error::TIMEOUT);
    }
}

fun modbus::api::Client::Diagnostic(uint8_t sub_function) -> std::vector<uint8_t> {
    uint8_t req[3] = {};
    req[0] = server_address;
    req[1] = DIAGNOSTIC;
    req[2] = sub_function;

    val res = request(req);
    val return_ = [this] (std::vector<uint8_t> res, Error err) { error_ = err; return res; };

    if (res.size() < 2)
        return return_({}, Error::TIMEOUT);
    
    return return_({res.begin() + 2, res.end()}, Error::NONE);
}

fun modbus::api::Client::WriteMultipleCoils(uint16_t register_address, uint16_t n_register, const std::vector<bool>& values) -> void {
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
    
    val res = request(req);
    val return_ = [this] (Error err) { error_ = err; };

    if (res.size() < 6)
        return return_(Error::TIMEOUT);
    
    if (not std::equal(res.begin(), res.begin() + 6, req.begin()))
        return return_(Error::DATA_FRAME);
    
    return return_(Error::NONE);
}

fun modbus::api::Client::WriteMultipleRegisters(uint16_t register_address, uint16_t n_register, const std::vector<uint16_t>& values) -> void {
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
    
    val res = request(req);
    val return_ = [this] (Error err) { error_ = err; };

    if (res.size() < 6)
        return return_(Error::TIMEOUT);
    
    if (not std::equal(res.begin(), res.begin() + 6, req.begin()))
        return return_(Error::DATA_FRAME);
    
    return return_(Error::NONE);
}

extern "C" {
    typedef void* nexus_modbus_client_t;

    void nexus_modbus_client_delete(nexus_modbus_client_t client) {
        delete static_cast<modbus::api::Client*>(client);
    }

    int* nexus_modbus_client_read_coil(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register) {
        var res = static_cast<modbus::api::Client*>(client)->ReadCoils(register_address, n_register);
        var ret = (int*) ::malloc(res.size());

        for (size_t i = 0; i < res.size(); ++i)
            ret[i] = res[i];
        
        return ret;
    }

    int* nexus_modbus_client_read_discrete_inputs(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register) {
        var res = static_cast<modbus::api::Client*>(client)->ReadDiscreteInputs(register_address, n_register);
        var ret = (int*) ::malloc(res.size() * sizeof(int));

        for (size_t i = 0; i < res.size(); ++i)
            ret[i] = res[i];
        
        return ret;
    }

    uint16_t* nexus_modbus_client_read_holding_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register) {
        var res = static_cast<modbus::api::Client*>(client)->ReadHoldingRegisters(register_address, n_register);
        var ret = (uint16_t*) ::malloc(res.size() * sizeof(uint16_t));

        ::memcpy(ret, res.data(), res.size());
        return ret;
    }

    uint16_t* nexus_modbus_client_read_input_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register) {
        var res = static_cast<modbus::api::Client*>(client)->ReadInputRegisters(register_address, n_register);
        var ret = (uint16_t*) ::malloc(res.size() * sizeof(uint16_t));

        ::memcpy(ret, res.data(), res.size());
        return ret;
    }

    int nexus_modbus_client_write_single_coil(nexus_modbus_client_t client, uint16_t register_address, int value) {
        return static_cast<modbus::api::Client*>(client)->WriteSingleCoil(register_address, value);
    }

    uint16_t nexus_modbus_client_write_single_register(nexus_modbus_client_t client, uint16_t register_address, uint16_t value) {
        return static_cast<modbus::api::Client*>(client)->WriteSingleRegister(register_address, value);
    }

    uint8_t nexus_modbus_client_read_exception_status(nexus_modbus_client_t client) {
        return static_cast<modbus::api::Client*>(client)->ReadExceptionStatus();
    }

    uint8_t* nexus_modbus_client_diagnostic(nexus_modbus_client_t client, uint8_t sub_function, size_t* length) {
        var res = static_cast<modbus::api::Client*>(client)->Diagnostic(sub_function);
        var ret = (uint8_t*) ::malloc(res.size());
        *length = res.size();

        ::memcpy(ret, res.data(), res.size());
        return ret;
    }

    void nexus_modbus_client_write_multiple_coils(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register, const int* values, size_t length) {
        static_cast<modbus::api::Client*>(client)->WriteMultipleCoils(register_address, n_register, std::vector<bool>(values, values + length));
    }

    void nexus_modbus_client_write_multiple_registers(nexus_modbus_client_t client, uint16_t register_address, uint16_t n_register, const uint16_t* values, size_t length) {
        static_cast<modbus::api::Client*>(client)->WriteMultipleRegisters(register_address, n_register, std::vector<uint16_t>(values, values + length));
    }
}