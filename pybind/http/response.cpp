#include "../pybind.h"
#include "nexus/http/response.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindHttpResponse(module_& m);
}

void pybind11::bindHttpResponse(module_& m) {
    class_<httplib::Response>(m, "HttpResponse", "HTTP Response")
    .def_readwrite("version",&httplib::Response::version)
    .def_readwrite("status", &httplib::Response::status)
    .def_readwrite("reason", &httplib::Response::reason)
    .def_readwrite("body", &httplib::Response::body)
    .def_readwrite("location", &httplib::Response::location)
    .def("has_header",
        &httplib::Response::has_header,
        arg("key")
    )
    .def("get_header_value",
        lambda (const httplib::Response& self, const std::string& key) -> std::string {
            return self.get_header_value(key);
        },
        arg("key")
    )
    .def("set_header",
        &httplib::Response::set_header,
        arg("key"),
        arg("value")
    )
    .def("set_content",
        overload_cast<const std::string&, const std::string&>(&httplib::Response::set_content),
        arg("body"),
        arg("content_type")
    );
}