#include "../../pybind.h"
#include "nexus/modbus/rtu/server.h"
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindModbusRTUServer(module_& m);
}

void pybind11::bindModbusRTUServer(module_& m) {
    class_<nexus::modbus::rtu::Server, nexus::modbus::api::Server>(m, "ModbusRTUServer", "Nexus RTU Server")
    .def(init<int>(),
        arg("address")
    )
    .def_property_readonly("logger", 
        &nexus::modbus::rtu::Server::setLogger,
        "set logger",
        return_value_policy::reference_internal
    )
    .def("listen",         
        [] (nexus::modbus::rtu::Server& self, const std::string& port, size_t speed, std::chrono::milliseconds timeout = 1s) {
            std::thread([&self, port, speed, timeout] { self.listen(port, speed, timeout); }).detach();
        },
        arg("host"),
        arg("port"),
        arg("timeout") = 1s,
        "Starts listening for incoming connections on a specific port.",
        return_value_policy::reference_internal
    )
    .def("stop", 
        &nexus::modbus::rtu::Server::stop,
        "Stops the server and closes all connections.",
        return_value_policy::reference_internal
    );
}