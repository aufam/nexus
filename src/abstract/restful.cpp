#include "nexus/abstract/restful.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

abstract::c_wrapper::Restful::~Restful() {
    if (c_members) {
        ::free(c_members);
        c_members = nullptr;
    }
}

fun abstract::c_wrapper::Restful::path() const -> std::string { 
    return get_restful_response(c_path, c_members); 
}

fun abstract::c_wrapper::Restful::json() const -> std::string { 
    return get_restful_response(c_json, c_members); 
}

fun abstract::c_wrapper::Restful::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return get_restful_response(c_post, c_members, std::string(method_name).c_str(), std::string(json_request).c_str());
}

fun abstract::c_wrapper::Restful::patch(std::string_view json_request) -> std::string {
    return get_restful_response(c_patch, c_members, std::string(json_request).c_str());
}

extern "C" {
    typedef void* nexus_restful_t;

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
        var res = new abstract::c_wrapper::Restful();
        res->c_path = path;
        res->c_json = json;
        res->c_post = post;
        res->c_patch = patch;
        res->c_members = members;
        return res;
    }

    fun nexus_restful_delete(nexus_restful_t restful) -> void {
        delete cast(restful);
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


