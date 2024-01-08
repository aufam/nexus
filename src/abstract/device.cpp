#include "nexus/abstract/device.h"
#include "nexus/abstract/c_wrapper.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace nexus;

cwrapper::Device::~Device() { delete restful; }

std::string cwrapper::Device::path() const { return get_restful_response(restful->cpath, restful->cmembers); }
std::string cwrapper::Device::json() const { return get_restful_response(restful->cjson, restful->cmembers); }

std::string cwrapper::Device::post(std::string_view method_name, std::string_view json_request) {
    return get_restful_response(restful->cpost, restful->cmembers, method_name.data(), json_request.data());
}

std::string cwrapper::Device::patch(std::string_view json_request) {
    return get_restful_response(restful->cpatch, restful->cmembers, json_request.data());
}

void cwrapper::Device::update() { if (cupdate) cupdate(restful->cmembers); }


extern "C" {
    fun static cast(nexus_device_t device) { 
        return static_cast<nexus::abstract::Device*>(device); 
    }

    nexus_device_t nexus_device_override_new(
        nexus_restful_t restful,
        void (*update)(void* members)
    ) {
        var res = new nexus::cwrapper::Device();
        res->restful = static_cast<nexus::cwrapper::Restful*>(restful);
        res->cupdate = update;
        return res;
    }

    void nexus_device_override_delete(nexus_device_t device) {
        delete static_cast<nexus::cwrapper::Device*>(device);
    }

    fun nexus_device_update(nexus_device_t device) -> void {
        cast(device)->update();
    }
}