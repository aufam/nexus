#include "../pybind.h"
#include "nexus/tcp/client.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindTCPClient(module_& m); 
}

void pybind11::bindTCPClient(module_& m) {
    class_<nexus::tcp::Client>(m, "TCPClient", "TCP Client")
    .def(init<std::string, int>(), 
        arg("host"), 
        arg("port")
    )
    .def("request", 
        [] (nexus::tcp::Client& self, const object& buffer, std::chrono::milliseconds timeout = std::chrono::milliseconds{1000L}) -> byte_view {
            val bv = byte_view(buffer);
            gil_scoped_release gil_release;
            return self.request(bv, timeout).first;
        },
        arg("buffer"), 
        arg("timeout") = std::chrono::milliseconds(1000),
        "Request"
    );
}