#include "nexus/abstract/communication.h"
#include "nexus/tools/json.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

fun abstract::Communication::json() const -> std::string {
    return "{"
        "\"isConnected\": " + std::string(isConnected() ? "true" : "false") +
    "}";
}

fun abstract::Communication::post(std::string_view method_name, std::string_view) -> std::string {
    if (method_name == "disconnect") {
        disconnect();
        return tools::json_response_status_success("Request to disconnect " + path());
    }

    if (method_name == "reconnect") {
        reconnect();
        return tools::json_response_status_success("Request to reconnect " + path());
    }

    return tools::json_response_status_fail_unknown_method();
}

abstract::c_wrapper::Communication::~Communication() { 
    delete restful; 
}

fun abstract::c_wrapper::Communication::path() const -> std::string { 
    return restful ? restful->path() : abstract::Communication::path(); 
}

fun abstract::c_wrapper::Communication::json() const -> std::string { 
    return restful ? restful->json() : abstract::Communication::json(); 
}

fun abstract::c_wrapper::Communication::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return restful ? restful->post(method_name, json_request) : abstract::Communication::post(method_name, json_request);
}

fun abstract::c_wrapper::Communication::patch(std::string_view json_request) -> std::string {
    return restful ? restful->patch(json_request) : abstract::Communication::patch(json_request);
}

void abstract::c_wrapper::Communication::reconnect() { 
    if (c_reconnect) c_reconnect(c_members); 
}

void abstract::c_wrapper::Communication::disconnect() { 
    if (c_disconnect) c_disconnect(c_members); 
}

bool abstract::c_wrapper::Communication::isConnected() const { 
    return c_isConnected && c_isConnected(c_members); 
}

extern "C" {
    typedef void* nexus_restful_t;
    typedef void* nexus_communication_t;

    fun static cast(nexus_communication_t communication) { 
        return static_cast<nexus::abstract::Communication*>(communication); 
    }

    fun nexus_communication_override_new(
            nexus_restful_t restful, 
            void (*reconnect)(void* members),
            void (*disconnect)(void* members),
            int (*isConnected)(void* members),
            void* members
    ) -> nexus_communication_t {
        var res = new abstract::c_wrapper::Communication();
        res->restful = static_cast<abstract::Restful*>(restful);
        res->c_reconnect = reconnect;
        res->c_disconnect = disconnect;
        res->c_isConnected = isConnected;
        res->c_members = members;
        return res;
    }

    fun nexus_communication_delete(nexus_communication_t communication) -> void {
        delete cast(communication);
    }

    void nexus_communication_reconnect(nexus_communication_t communication) {
        cast(communication)->reconnect();
    }

    void nexus_communication_disconnect(nexus_communication_t communication) {
        cast(communication)->disconnect();
    }
    
    int nexus_communication_is_connected(nexus_communication_t communication) {
        return cast(communication)->isConnected();
    }
}
