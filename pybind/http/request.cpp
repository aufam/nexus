#include "../pybind.h"
#include "nexus/http/request.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindHttpRequest(module_& m);
}

void pybind11::bindHttpRequest(module_& m) {
    class_<httplib::Request>(m, "HttpRequest", "HTTP Request")
    .def_readwrite("method", &httplib::Request::method)
    .def_readwrite("version", &httplib::Request::version)
    .def_readwrite("path", &httplib::Request::path)
    .def_readwrite("body", &httplib::Request::body)
    .def_readwrite("remote_addr", &httplib::Request::remote_addr)
    .def_readwrite("remote_port", &httplib::Request::remote_port)
    .def_readwrite("local_port", &httplib::Request::local_port)
    .def_readwrite("local_addr", &httplib::Request::local_addr)
    .def_property_readonly("params", lambda (const httplib::Request& self) {
        var res = std::unordered_map<std::string, std::vector<std::string>>();
        for (auto &[key, value] in self.params) {
            res[key].push_back(value);
        }
        return res;
    })
    .def("has_header",
        &httplib::Request::has_header,
        arg("key")
    )
    .def("get_header_value",
        lambda (const httplib::Request& self, const std::string& key) -> std::string {
            return self.get_header_value(key);
        },
        arg("key")
    )
    .def("set_header",
        &httplib::Request::set_header,
        arg("key"),
        arg("value")
    );
}