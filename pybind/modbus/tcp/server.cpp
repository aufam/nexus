#include "../../pybind.h"
#include "nexus/modbus/tcp/server.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusTCPServer(module_& m);
}

void pybind11::bindModbusTCPServer(module_& m) {
    using LoggerFunction = std::function<void(const char*, byte_view, byte_view)>;

    class_<nexus::modbus::tcp::Server>(m, "ModbusTCPServer", "Nexus TCP Server")
    .def(init<>())
    .def("CoilGetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.CoilGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("CoilSetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(bool)> setter) {
                self.CoilSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterGetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.HoldingRegisterGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterSetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(uint16_t)> setter) {
                self.HoldingRegisterSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("DiscreteInputGetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.DiscreteInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("AnalogInputGetter",
        [] (nexus::modbus::tcp::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.AnalogInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def_property_readonly("ExceptionStatusGetter",
        [] (nexus::modbus::tcp::Server& self) {
            return std::function([&self] (std::function<uint8_t()> getter) {
                self.ExceptionStatusGetter(std::move(getter));
            });
        }
    )
    .def("DiagnosticGetter",
        [] (nexus::modbus::tcp::Server& self, uint8_t sub_function) {
            return std::function([&self, sub_function] (std::function<std::vector<uint8_t>()> getter) {
                self.DiagnosticGetter(sub_function, std::move(getter));
            });
        },
        arg("sub_function")
    )
    .def_property_readonly("logger", 
        [] (nexus::modbus::tcp::Server& self) {
            return std::function([&self] (LoggerFunction logger) {
                self.setLogger(std::move(logger));
            });
        },
        "set logger",
        return_value_policy::reference_internal
    )
    .def("listen",         
        [] (nexus::modbus::tcp::Server& self, const std::string& host, int port) {
            std::thread([&self, host, port] {
                self.listen(host, port);
            }).detach();
        },
        arg("host"),
        arg("port"),
        "Starts listening for incoming connections on a specific port.",
        return_value_policy::reference_internal
    )
    .def("stop", 
        &nexus::modbus::tcp::Server::stop,
        "Stops the server and closes all connections.",
        return_value_policy::reference_internal
    );
}