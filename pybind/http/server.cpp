#include "../pybind.h"
#include "nexus/http/server.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindHttpServer(module_& m);
}

void pybind11::bindHttpServer(module_& m) {
    using HandlerFunction = std::function<httplib::Response(httplib::Request, httplib::Response)>;
    using LoggerFunction = std::function<void(httplib::Request, httplib::Response)>;

    class_<nexus::http::Server, std::shared_ptr<nexus::http::Server>>(m, "HttpServer", "Nexus HTTP Server")
    .def(init<>())
    .def("add", 
        overload_cast<std::shared_ptr<nexus::abstract::Restful>, std::string, std::unordered_map<std::string, std::string>>(&nexus::http::Server::add),
        arg("restful"),
        arg("base_path") = "",
        arg("query") = dict(),
        "Adds a Nexus restful to the server's context.",
        return_value_policy::reference_internal
    )
    .def("Get", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Get(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP GET method.",
        return_value_policy::reference_internal
    )
    .def("Post", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Post(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP POST method.",
        return_value_policy::reference_internal
    )
    .def("Put", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Put(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP PUT method.",
        return_value_policy::reference_internal
    )
    .def("Patch", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Patch(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP PATCH method.",
        return_value_policy::reference_internal
    )
    .def("Delete", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Delete(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP DELETE method.",
        return_value_policy::reference_internal
    )
    .def("Options", 
        [] (nexus::http::Server& self, const std::string& path) {
            std::function res = [&self, path] (HandlerFunction handler) {
                self.Options(path, [handler] (const httplib::Request &req, httplib::Response &res) {
                    res = handler(req, res);
                });
            };
            return res;
        },
        arg("path"),
        "Adds a handler for HTTP OPTIONS method.",
        return_value_policy::reference_internal
    )
    .def("listen", 
        [] (nexus::http::Server& self, const std::string& host, int port) {
            std::thread([&self, host, port] { self.listen(host, port); }).detach();
        },
        arg("host"),
        arg("port"),
        "Starts listening for incoming connections on a specific port.",
        return_value_policy::reference_internal
    )
    .def("stop", 
        [] (nexus::http::Server& self) {
            gil_scoped_release release;
            self.stop();
        },
        "Stops the server and closes all connections.",
        return_value_policy::reference_internal
    )
    .def_property_readonly("is_running", 
        &nexus::http::Server::is_running,
        "Checks if the server is currently running.",
        return_value_policy::reference_internal
    )
    .def_property_readonly("logger", 
        [] (nexus::http::Server& self) {
            return std::function([&self] (LoggerFunction logger) {
                self.set_logger([logger] (const httplib::Request &req, const httplib::Response &res) {
                    logger(req, res);
                });
            });
        },
        "set logger",
        return_value_policy::reference_internal
    );
}