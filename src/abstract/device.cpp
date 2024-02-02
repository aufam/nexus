#include "nexus/abstract/device.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

abstract::c_wrapper::Device::~Device() { 
    delete restful; 
}

fun abstract::c_wrapper::Device::path() const -> std::string { 
    return restful ? restful->path() : abstract::Device::path(); 
}

fun abstract::c_wrapper::Device::json() const -> std::string { 
    return restful ? restful->json() : abstract::Device::json(); 
}

fun abstract::c_wrapper::Device::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return restful ? restful->post(method_name, json_request) : abstract::Device::post(method_name, json_request);
}

fun abstract::c_wrapper::Device::patch(std::string_view json_request) -> std::string {
    return restful ? restful->patch(json_request) : abstract::Device::patch(json_request);
}

void abstract::c_wrapper::Device::update() { 
    if (c_update) c_update(c_members); 
}

extern "C" {
    typedef void* nexus_restful_t;
    typedef void* nexus_device_t;

    fun static cast(nexus_device_t device) { 
        return static_cast<abstract::Device*>(device); 
    }

    nexus_device_t nexus_device_override_new(
        nexus_restful_t restful,
        void (*update)(void* members),
        void* members
    ) {
        var res = new abstract::c_wrapper::Device();
        res->restful = static_cast<abstract::Restful*>(restful);
        res->c_update = update;
        res->c_members = members;
        return res;
    }

    void nexus_device_delete(nexus_device_t device) {
        delete cast(device);
    }

    fun nexus_device_update(nexus_device_t device) -> void {
        cast(device)->update();
    }
}