#include "nexus/abstract/restful.h"
#include "nexus/abstract/c_wrapper.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

cwrapper::Restful::~Restful() {
    if (cmembers) {
        ::free(cmembers);
        cmembers = nullptr;
    }
}

fun cwrapper::Restful::path() const -> std::string { 
    return get_restful_response(cpath, cmembers); 
}

fun cwrapper::Restful::json() const -> std::string { 
    return get_restful_response(cjson, cmembers); 
}

fun cwrapper::Restful::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return get_restful_response(cpost, cmembers, method_name.data(), json_request.data());
}

fun cwrapper::Restful::patch(std::string_view json_request) -> std::string {
    return get_restful_response(cpatch, cmembers, json_request.data());
}

extern "C" {
    fun static cast(nexus_restful_t restful) { 
        return static_cast<nexus::abstract::Restful*>(restful); 
    }

    fun nexus_restful_override_new(
        char* (*path)(void* members),
        char* (*json)(void* members),
        char* (*post)(void* members, const char* method_name, const char* json_request),
        char* (*patch)(void* members, const char* json_request),
        void* members
    ) -> nexus_restful_t {
        var res = new nexus::cwrapper::Restful();
        res->cpath = path;
        res->cjson = json;
        res->cpost = post;
        res->cpatch = patch;
        res->cmembers = members;
        return res;
    }

    fun nexus_restful_override_delete(nexus_restful_t restful) -> void {
        delete static_cast<nexus::cwrapper::Restful*>(restful);
    }

    fun nexus_restful_path(nexus_restful_t restful) -> char* { 
        return c_string(cast(restful)->path());
    }

    fun nexus_restful_json(nexus_restful_t restful) -> char* {
        return c_string(cast(restful)->json());
    }

    fun nexus_restful_post(nexus_restful_t restful, const char* method_name, const char* json_request) -> char* {
        return c_string(cast(restful)->post(method_name, json_request));
    }

    fun nexus_restful_patch(nexus_restful_t restful, const char* json_request) -> char* {
        return c_string(cast(restful)->patch(json_request));
    }
}


