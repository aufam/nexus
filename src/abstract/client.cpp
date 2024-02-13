#include "nexus/abstract/client.h"
#include "nexus/tools/json.h"
#include "nexus/tools/to_string.h"
#include "helper.ipp"
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

fun abstract::Client::post(std::string_view method_name, std::string_view json_request) -> std::string {
    val json = tools::json_parse(json_request);
    
    if (method_name == "request") {
        val buffer = json["buffer"];
        if (not buffer.is_list())
            return tools::json_response_status_fail_mismatch_value_type();
        
        val req = tools::json_to_vector<uint8_t>(buffer);
        val res = tools::to_string(request(req).to_vector());

        return tools::json_concat(tools::json_response_status_success("Request"), "{"
            "\"res\": " + res + 
        "}");
    }

    return abstract::Communication::post(method_name, json_request);
}

abstract::c_wrapper::Client::~Client() {
    delete communication;
}
fun abstract::c_wrapper::Client::path() const -> std::string { 
    return communication ? communication->path() : abstract::Client::path(); 
}

fun abstract::c_wrapper::Client::json() const -> std::string { 
    return communication ? communication->json() : abstract::Client::json(); 
}

fun abstract::c_wrapper::Client::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return communication ? communication->post(method_name, json_request) : abstract::Client::post(method_name, json_request);
}

fun abstract::c_wrapper::Client::patch(std::string_view json_request) -> std::string {
    return communication ? communication->patch(json_request) : abstract::Client::patch(json_request);
}

void abstract::c_wrapper::Client::reconnect() { 
    if (communication) communication->reconnect(); 
}

void abstract::c_wrapper::Client::disconnect() { 
    if (communication) communication->disconnect(); 
}

bool abstract::c_wrapper::Client::isConnected() const { 
    return communication && communication->isConnected(); 
}

fun abstract::c_wrapper::Client::request(byte_view buffer) -> byte_view { 
    size_t length = buffer.len();
    uint8_t* res = c_request ? c_request(c_members, buffer.data(), &length) : nullptr;
    etl::ScopeExit se([res] { std::free(res); });
    return std::move(std::vector(res, res + length));
}

extern "C" {
    typedef void* nexus_client_t;
    typedef void* nexus_communication_t;

    fun static cast(nexus_client_t client) { 
        return static_cast<abstract::Client*>(client); 
    }

    nexus_client_t nexus_client_override_new(
        nexus_communication_t communication,
        uint8_t* (*request)(void* members, const uint8_t* buffer, size_t* length),
        void* members
    ) {
        var res = new abstract::c_wrapper::Client();
        res->communication = static_cast<abstract::c_wrapper::Communication*>(communication);
        res->c_request = request;
        res->c_members = members;
        return res;
    }

    void nexus_client_delete(nexus_client_t client) {
        delete cast(client); 
    }

    uint8_t* nexus_client_request(nexus_client_t client, const uint8_t* buffer, size_t* length) {
        val res = cast(client)->request(byte_view{buffer, *length});
        *length = res.len();
        
        uint8_t* ret = (uint8_t*) ::malloc(*length);
        ::memcpy(ret, res.data(), *length);
        return ret;
    }
}