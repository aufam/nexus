#include "../../pybind.h"
#include "nexus/modbus/tcp/server.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusTCPServer(module_& m);
}

void pybind11::bindModbusTCPServer(module_& m) {
    class_<nexus::modbus::tcp::Server, nexus::modbus::api::Server, nexus::tcp::Server, std::shared_ptr<nexus::modbus::tcp::Server>>(m, 
        "ModbusTCPServer"
    )
    .def(init<>());
}