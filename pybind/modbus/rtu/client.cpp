#include "../../pybind.h"
#include "nexus/modbus/rtu/client.h"
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindModbusRTUClient(module_& m); 
}

void pybind11::bindModbusRTUClient(module_& m) {
    class_<nexus::modbus::rtu::Client, nexus::modbus::api::Client, nexus::serial::Hardware::Interface>(m, "ModbusRTUClient", "Modbus RTU Client")
    .def(init<int, std::string, speed_t, std::chrono::milliseconds>(), 
        arg("server_address"), 
        arg("port"), 
        arg("speed"),
        arg("timeout") = 100ms
    );
}