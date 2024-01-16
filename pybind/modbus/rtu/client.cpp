#include "../../pybind.h"
#include "nexus/modbus/rtu/client.h"
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindModbusRTUClient(module_& m); 
}

void pybind11::bindModbusRTUClient(module_& m) {
    class_<nexus::modbus::rtu::Client, nexus::abstract::Communication>(m, "ModbusRTUClient", "Modbus RTU Client")
    .def(init<std::string, BaudRate, std::chrono::milliseconds>(), 
        arg("port"), 
        arg("speed"),
        arg("timeout") = 100ms
    )
    .def("ReadCoils", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadCoils(server_address, register_address, n_register);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadDiscreteInputs", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadDiscreteInputs(server_address, register_address, n_register);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadHoldingRegisters", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadHoldingRegisters(server_address, register_address, n_register);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadInputRegisters", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadInputRegisters(server_address, register_address, n_register);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register")
    )
    .def("WriteSingleCoil", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, bool value) {
            gil_scoped_release gil_release;
            return self.WriteSingleCoil(server_address, register_address, value);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("value")
    )
    .def("WriteSingleRegister", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t value) {
            gil_scoped_release gil_release;
            return self.WriteSingleRegister(server_address, register_address, value);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("value")
    )
    .def("ReadExceptionStatus", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address) {
            gil_scoped_release gil_release;
            return self.ReadExceptionStatus(server_address);
        },
        arg("server_address")
    )
    .def("Diagnostic", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint8_t sub_function) {
            gil_scoped_release gil_release;
            return self.Diagnostic(server_address, sub_function);
        },
        arg("server_address"), 
        arg("sub_function")
    )
    .def("WriteMultipleCoils", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<bool> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleCoils(server_address, register_address, n_register, values);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    )
    .def("WriteMultipleRegisters", 
        [] (nexus::modbus::rtu::Client& self, uint8_t server_address, uint16_t register_address, uint16_t n_register, const std::vector<uint16_t> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleRegisters(server_address, register_address, n_register, values);
        },
        arg("server_address"), 
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    )
    .def("Request", 
        [] (nexus::modbus::rtu::Client& self, const std::vector<uint8_t>& data) {
            gil_scoped_release gil_release;
            return self.Request(data);
        },
        arg("data")
    );
}