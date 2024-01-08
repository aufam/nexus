#include "nexus/abstract/communication.h"
#include "nexus/abstract/c_wrapper.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

cwrapper::Communication::~Communication() { 
    delete restful; 
}

fun cwrapper::Communication::path() const -> std::string { 
    return get_restful_response(restful->cpath, restful->cmembers); 
}

fun cwrapper::Communication::json() const -> std::string { 
    return get_restful_response(restful->cjson, restful->cmembers); 
}

fun cwrapper::Communication::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return get_restful_response(restful->cpost, restful->cmembers, method_name.data(), json_request.data());
}

fun cwrapper::Communication::patch(std::string_view json_request) -> std::string {
    return get_restful_response(restful->cpatch, restful->cmembers, json_request.data());
}

extern "C" {

    fun nexus_communication_override_new(nexus_restful_t restful) -> nexus_communication_t {
        var res = new nexus::cwrapper::Communication();
        res->restful = static_cast<nexus::cwrapper::Restful*>(restful);
        return res;
    }

    fun nexus_communication_override_delete(nexus_communication_t communication) -> void {
        delete static_cast<nexus::cwrapper::Communication*>(communication);
    }
}
