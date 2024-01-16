#include "../../pybind.h"
#include "nexus/modbus/rtu/server.h"
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindModbusRTUServer(module_& m);
}

void pybind11::bindModbusRTUServer(module_& m) {
    class_<nexus::modbus::rtu::Server, nexus::abstract::Communication>(m, "ModbusRTUServer", "Nexus RTU Server")
    .def(init<uint8_t, std::string, BaudRate, std::chrono::milliseconds>(),
        arg("address"),
        arg("port"), 
        arg("speed"),
        arg("timeout") = 100ms
    )
    .def("CoilGetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.CoilGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("CoilSetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(bool)> setter) {
                self.CoilSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterGetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.HoldingRegisterGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterSetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(uint16_t)> setter) {
                self.HoldingRegisterSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("DiscreteInputGetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.DiscreteInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("AnalogInputGetter",
        [] (nexus::modbus::rtu::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.AnalogInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def_property_readonly("ExceptionStatusGetter",
        [] (nexus::modbus::rtu::Server& self) {
            return std::function([&self] (std::function<uint8_t()> getter) {
                self.ExceptionStatusGetter(std::move(getter));
            });
        }
    )
    .def("DiagnosticGetter",
        [] (nexus::modbus::rtu::Server& self, uint8_t sub_function) {
            return std::function([&self, sub_function] (std::function<std::vector<uint8_t>()> getter) {
                self.DiagnosticGetter(sub_function, std::move(getter));
            });
        },
        arg("sub_function")
    )
    .def_property_readonly("callback",
        [] (nexus::modbus::rtu::Server& self) {
            return std::function ([&self] (std::function<void(byte_view)> callback) {
                self.addCallback(std::move(callback));
            });
        },
        "Adds a callback function to be invoked when data is received."
    );
}