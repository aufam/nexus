#include "../pybind.h"
#include "nexus/http/client.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindHttpClient(module_& m); 
}

void pybind11::bindHttpClient(module_& m) {
    class_<nexus::http::Client, std::shared_ptr<nexus::http::Client>>(m, "HttpClient", "HTTP Client")
    .def(init<std::string, int>(), 
        arg("host"), 
        arg("port")
    )
    .def(init<std::string>(), 
        arg("host_port")
    )
    .def("Get", 
        [] (nexus::http::Client& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Get(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        }, 
        arg("path"),
        "Sends an HTTP GET request and returns the response."
    )
    .def("Head",
        [] (nexus::http::Client& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Head(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        "Sends an HTTP HEAD request and returns the response."
    )
    .def("Post",
        [] (nexus::http::Client& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Post(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        arg("body") = "",
        arg("content_type") = "",
        "Sends an HTTP POST request and returns the response."
    )
    .def("Put",
        [] (nexus::http::Client& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Put(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        arg("body") = "",
        arg("content_type") = "",
        "Sends an HTTP PUT request and returns the response."
    )
    .def("Patch",
        [] (nexus::http::Client& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Patch(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        arg("body") = "",
        arg("content_type") = "",
        "Sends an HTTP PATCH request and returns the response."
    )
    .def("Delete",
        [] (nexus::http::Client& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Delete(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        arg("body") = "",
        arg("content_type") = "",
        "Sends an HTTP DELETE request and returns the response."
    )
    .def("Options",
        [] (nexus::http::Client& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Options(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        arg("path"),
        "Sends an HTTP Options request and returns the response."
    );
}