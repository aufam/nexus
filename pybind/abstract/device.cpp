#include "../pybind.h"
#include "nexus/abstract/device.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindDevice(module_& m); 

    class Device : virtual public nexus::abstract::Device {
    public:
        Device() = default;
        virtual ~Device() {}

        void update() override {
            PYBIND11_OVERRIDE(void, nexus::abstract::Device, update, );
        }
        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Device, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Device, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Device, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Device, post, method_name, json_request);
        }
    };
}

void pybind11::bindDevice(module_& m) {
    class_<nexus::abstract::Device, Device, nexus::abstract::Restful>(m, "Device", "Abstract Device")
    .def(init<>())
    .def("update",
        &nexus::abstract::Device::update,
        "Updates all device parameters."
    );
}
