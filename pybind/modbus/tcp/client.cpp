#include "../../pybind.h"
#include "nexus/modbus/tcp/client.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusTCPClient(module_& m); 
}

void pybind11::bindModbusTCPClient(module_& m) {
    class_<nexus::modbus::tcp::Client>(m, "ModbusTCPClient", "Modbus TCP Client")
    .def(init<std::string, int>(), 
        arg("host"), 
        arg("port")
    )
    .def("ReadCoils", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadCoils(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadDiscreteInputs", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadDiscreteInputs(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadHoldingRegisters", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadHoldingRegisters(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("ReadInputRegisters", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register) {
            gil_scoped_release gil_release;
            return self.ReadInputRegisters(register_address, n_register);
        },
        arg("register_address"), 
        arg("n_register")
    )
    .def("WriteSingleCoil", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, bool value) {
            gil_scoped_release gil_release;
            return self.WriteSingleCoil(register_address, value);
        },
        arg("register_address"), 
        arg("value")
    )
    .def("WriteSingleRegister", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t value) {
            gil_scoped_release gil_release;
            return self.WriteSingleRegister(register_address, value);
        },
        arg("register_address"), 
        arg("value")
    )
    .def("ReadExceptionStatus", 
        [] (nexus::modbus::tcp::Client& self) {
            gil_scoped_release gil_release;
            return self.ReadExceptionStatus();
        }
    )
    .def("Diagnostic", 
        [] (nexus::modbus::tcp::Client& self, uint8_t sub_function) {
            gil_scoped_release gil_release;
            return self.Diagnostic(sub_function);
        },
        arg("sub_function")
    )
    .def("WriteMultipleCoils", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register, const std::vector<bool> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleCoils(register_address, n_register, values);
        },
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    )
    .def("WriteMultipleRegisters", 
        [] (nexus::modbus::tcp::Client& self, uint16_t register_address, uint16_t n_register, const std::vector<uint16_t> &values) {
            gil_scoped_release gil_release;
            return self.WriteMultipleRegisters(register_address, n_register, values);
        },
        arg("register_address"), 
        arg("n_register"),
        arg("values")
    );
}