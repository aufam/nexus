#include "nexus/abstract/serial.h"
#include "nexus/tools/json.h"
#include "nexus/tools/to_string.h"
#include "helper.ipp"
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

fun abstract::Serial::post(std::string_view method_name, std::string_view json_request) -> std::string {
    val json = tools::json_parse(json_request);
    
    if (method_name == "send") {
        val buffer = json["buffer"];
        if (not buffer.is_list())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val res = send(tools::json_to_vector<uint8_t>(buffer));
        return tools::json_concat(tools::json_response_status_success("Request to send"), "{"
            "\"res\": " + tools::to_string(res) + 
        "}");
    }

    if (method_name == "receive") {
        val res = receive();
        return tools::json_concat(tools::json_response_status_success("Request to receive"), "{"
            "\"res\": " + tools::to_string(res.to_vector()) + 
        "}");
    }

    return abstract::Communication::post(method_name, json_request);
}

abstract::c_wrapper::Serial::~Serial() {
    delete communication;
}

fun abstract::c_wrapper::Serial::path() const -> std::string { 
    return communication ? communication->path() : abstract::Serial::path(); 
}

fun abstract::c_wrapper::Serial::json() const -> std::string { 
    return communication ? communication->json() : abstract::Serial::json(); 
}

fun abstract::c_wrapper::Serial::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return communication ? communication->post(method_name, json_request) : abstract::Serial::post(method_name, json_request);
}

fun abstract::c_wrapper::Serial::patch(std::string_view json_request) -> std::string {
    return communication ? communication->patch(json_request) : abstract::Serial::patch(json_request);
}

void abstract::c_wrapper::Serial::reconnect() { 
    if (communication) communication->reconnect(); 
}

void abstract::c_wrapper::Serial::disconnect() { 
    if (communication) communication->disconnect(); 
}

bool abstract::c_wrapper::Serial::isConnected() const { 
    return communication && communication->isConnected(); 
}

int abstract::c_wrapper::Serial::send(byte_view buffer) { 
    return c_send ? c_send(c_members, buffer.data(), buffer.len()) : -1; 
}

fun abstract::c_wrapper::Serial::receive() -> byte_view { 
    size_t length;
    uint8_t* res = c_receive ? c_receive(c_members, &length) : nullptr; 
    etl::ScopeExit se([res] { std::free(res); });
    return std::move(std::vector(res, res + length));
}

fun abstract::c_wrapper::Serial::receive(std::function<bool(byte_view)> filter) -> byte_view {
    while (true) {
        var res = receive();
        if (filter(res))
            return res;
    }
}

extern "C" {
    typedef void* nexus_serial_t;
    typedef void* nexus_communication_t;

    fun static cast(nexus_serial_t serial) { 
        return static_cast<abstract::Serial*>(serial); 
    }

    nexus_serial_t nexus_serial_override_new(
        nexus_communication_t communication,
        int (*send)(void* members, const uint8_t* buffer, size_t length),
        uint8_t* (*receive)(void* members, size_t* length),
        void* members
    ) {
        var res = new abstract::c_wrapper::Serial();
        res->communication = static_cast<abstract::c_wrapper::Communication*>(communication);
        res->c_send = send;
        res->c_receive = receive;
        res->c_members = members;
        return res;
    }

    void nexus_serial_delete(nexus_serial_t serial) {
        delete cast(serial); 
    }

    int nexus_serial_send(nexus_serial_t serial, const uint8_t* buffer, size_t length) {
        return cast(serial)->send(byte_view{buffer, length});
    }

    uint8_t* nexus_serial_receive(nexus_serial_t serial, size_t* length) {
        var res = cast(serial)->receive();
        *length = res.len();
        
        uint8_t* ret = (uint8_t*) ::malloc(*length);
        ::memcpy(ret, res.data(), *length);
        return ret;
    }

    uint8_t* nexus_serial_receive_filter(nexus_serial_t serial, size_t* length, int (*filter) (const uint8_t* buffer, size_t length)) {
        var res = cast(serial)->receive([filter] (byte_view buffer) { return filter(buffer.data(), buffer.len()); });
        *length = res.len();
        
        uint8_t* ret = (uint8_t*) ::malloc(*length);
        ::memcpy(ret, res.data(), *length);
        return ret;
    }
}