#include "../pybind.h"
#include "nexus/tcp/server.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindTCPServer(module_& m);
}

void pybind11::bindTCPServer(module_& m) {
    using HandlerFunction = std::function<std::vector<uint8_t>(nexus::byte_view)>;
    using LoggerFunction = std::function<void(const char*, nexus::byte_view, nexus::byte_view)>;

    class_<nexus::tcp::Server>(m, "TCPServer", "Nexus TCP Server")
    .def(init<>())
    .def_property_readonly("callback", 
        [] (nexus::tcp::Server& self) {
            return std::function([&self] (HandlerFunction callback) {
                self.addCallback(std::move(callback));
            });
        },
        "set logger",
        return_value_policy::reference_internal
    )
    .def_property_readonly("logger", 
        [] (nexus::tcp::Server& self) {
            return std::function([&self] (LoggerFunction logger) {
                self.setLogger(std::move(logger));
            });
        },
        "set logger",
        return_value_policy::reference_internal
    )
    .def("listen",         
        [] (nexus::tcp::Server& self, const std::string& host, int port) {
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
        &nexus::tcp::Server::stop,
        "Stops the server and closes all connections.",
        return_value_policy::reference_internal
    );
}