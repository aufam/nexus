#include "../pybind.h"
#include "nexus/modbus/api.h"
#include "nexus/modbus/api_client.h"
#include "nexus/modbus/api_server.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusAPI(module_& m); 
}

void pybind11::bindModbusAPI(module_& m) {
    enum_<nexus::modbus::Error>(m, "ModbusError")
    .value("NONE", nexus::modbus::Error::NONE)
    .value("TIMEOUT", nexus::modbus::Error::TIMEOUT)
    .value("DATA_FRAME", nexus::modbus::Error::DATA_FRAME)
    .export_values();

    class_<nexus::modbus::api::Codec, nexus::abstract::Codec>(m, "ModbusCodec", "Modbus Codec").def(init<>());
    m.def("ModbusCRC", nexus::modbus::api::crc, arg("buffer"));

    class_<nexus::modbus::api::Client, nexus::abstract::Client>(m, "ModbusClient")
    .def("ReadCoils", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadCoils(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadDiscreteInputs", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadDiscreteInputs(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadHoldingRegisters", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadHoldingRegisters(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadInputRegisters", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadInputRegisters(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("WriteSingleCoil", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, bool value) {
            gil_scoped_release gil_release;
            return self.WriteSingleCoil(register_address, value);
        },
        arg("register_address"), 
        arg("value")
    )
    .def("WriteSingleRegister", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t value) {
            gil_scoped_release gil_release;
            return self.WriteSingleRegister(register_address, value);
        },
        arg("register_address"), 
        arg("value")
    )
    .def("ReadExceptionStatus", 
        [] (nexus::modbus::api::Client& self) {
            gil_scoped_release gil_release;
            return self.ReadExceptionStatus();
        }
    )
    .def("Diagnostic", 
        [] (nexus::modbus::api::Client& self, uint8_t sub_function) {
            gil_scoped_release gil_release;
            return self.Diagnostic(sub_function);
        },
        arg("sub_function")
    )
    .def("WriteMultipleCoils", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register, const std::vector<bool> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleCoils(register_address, n_register, values);
        },
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    )
    .def("WriteMultipleRegisters", 
        [] (nexus::modbus::api::Client& self, uint16_t register_address, uint16_t n_register, const std::vector<uint16_t> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleRegisters(register_address, n_register, values);
        },
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    )
    .def("error", &nexus::modbus::api::Client::error);

    class_<nexus::modbus::api::Server, nexus::abstract::Restful>(m, "ModbusServer")
    .def("isRunning", &nexus::modbus::api::Server::isRunning)
    .def("CoilGetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.CoilGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("CoilSetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(bool)> setter) {
                self.CoilSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterGetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.HoldingRegisterGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("HoldingRegisterSetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<void(uint16_t)> setter) {
                self.HoldingRegisterSetter(register_address, std::move(setter));
            });
        },
        arg("register_address")
    )
    .def("DiscreteInputGetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<bool()> getter) {
                self.DiscreteInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def("AnalogInputGetter",
        [] (nexus::modbus::api::Server& self, uint16_t register_address) {
            return std::function([&self, register_address] (std::function<uint16_t()> getter) {
                self.AnalogInputGetter(register_address, std::move(getter));
            });
        },
        arg("register_address")
    )
    .def_property_readonly("ExceptionStatusGetter",
        [] (nexus::modbus::api::Server& self) {
            return std::function([&self] (std::function<uint8_t()> getter) {
                self.ExceptionStatusGetter(std::move(getter));
            });
        }
    )
    .def("DiagnosticGetter",
        [] (nexus::modbus::api::Server& self, uint8_t sub_function) {
            return std::function([&self, sub_function] (std::function<std::vector<uint8_t>()> getter) {
                self.DiagnosticGetter(sub_function, std::move(getter));
            });
        },
        arg("sub_function")
    );
}