#include "../pybind.h"
#include "nexus/http/client.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindClient(module_& m); 

    class HttpClient : virtual public nexus::http::Client {
    public:
        HttpClient(std::string host, int port) : nexus::http::Client(host, port) {}
        HttpClient(std::string host_port) : nexus::http::Client(host_port) {}
        virtual ~HttpClient() {}

        std::string path() const override {
            return _path ? _path() : nexus::http::Client::path();
        }
        std::string json() const override {
            return _json ? _json() : nexus::http::Client::json();
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            return _post ? _post(method_name, json_request) : nexus::http::Client::post(method_name, json_request);
        }
        std::string patch(std::string_view json_request) override {
            return _patch ? _patch(json_request) : nexus::http::Client::patch(json_request);
        }

        std::function<std::string()> _path;
        std::function<std::string()> _json;
        std::function<std::string(std::string_view, std::string_view)> _post;
        std::function<std::string(std::string_view)> _patch;
    };
}

void pybind11::bindClient(module_& m) {
    class_<HttpClient, nexus::abstract::Communication>(m, "HttpClient", "HTTP Client")
    .def(init<std::string, int>(), 
        arg("host"), 
        arg("port")
    )
    .def(init<std::string>(), 
        arg("host_port")
    )
    .def("Get", 
        [] (HttpClient& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Get(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        }, 
        "Sends an HTTP GET request and returns the response."
    )
    .def("Head",
        [] (HttpClient& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Head(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP HEAD request and returns the response."
    )
    .def("Post",
        [] (HttpClient& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Post(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP POST request and returns the response."
    )
    .def("Put",
        [] (HttpClient& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Put(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP PUT request and returns the response."
    )
    .def("Patch",
        [] (HttpClient& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Patch(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP PATCH request and returns the response."
    )
    .def("Delete",
        [] (HttpClient& self, const std::string& path, const std::string& body, const std::string& content_type) {
            gil_scoped_release gil_release;
            var res = self.Delete(path, body, content_type);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP DELETE request and returns the response."
    )
    .def("Options",
        [] (HttpClient& self, const std::string& path) {
            gil_scoped_release gil_release;
            var res = self.Options(path);
            return res ? std::move(*res.operator->()) : httplib::Response{};
        },
        "Sends an HTTP Options request and returns the response."
    )
    .def_property_readonly("path_override",
        [] (HttpClient& self) {
            return std::function ([&self] (const std::function<std::string()>& path) {
                self._path = path;
            });
        },
        "Override path as function decorator"
    )
    .def_property_readonly("json_override",
        [] (HttpClient& self) {
            return std::function ([&self] (const std::function<std::string()>& json) {
                self._json = json;
            });
        },
        "Override json as function decorator"
    )
    .def_property_readonly("post_override",
        [] (HttpClient& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view, std::string_view)>& post) {
                self._post = post;
            });
        },
        "Override post as function decorator"
    )
    .def_property_readonly("patch_override",
        [] (HttpClient& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view)>& patch) {
                self._patch = patch;
            });
        },
        "Override patch as function decorator"
    );
}