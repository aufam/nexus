#include "../../pybind.h"
#include "nexus/modbus/tcp/client.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusTCPClient(module_& m); 
}

void pybind11::bindModbusTCPClient(module_& m) {
    class_<nexus::modbus::tcp::Client, nexus::modbus::api::Client, std::shared_ptr<nexus::modbus::tcp::Client>>(m, "ModbusTCPClient")
    .def(init<std::string, int, std::chrono::milliseconds>(), 
        arg("host"), 
        arg("port"),
        arg("timeout") = std::chrono::milliseconds(1000)
    );
}