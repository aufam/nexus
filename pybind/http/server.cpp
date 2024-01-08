#include "../pybind.h"
#include "nexus/http/server.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindServer(module_& m);

    class HttpServer : public nexus::http::Server {
    public:
        using nexus::http::Server::Server;
        virtual ~HttpServer() {}
        
        void listen(const std::string& host, int port) {
            std::thread([this, host, port] {
                nexus::http::Server::listen(host, port);
            }).detach();
        }
    };
}

void pybind11::bindServer(module_& m) {
    using HandlerFunction = std::function<httplib::Response(httplib::Request, httplib::Response)>;
    using LoggerFunction = std::function<void(httplib::Request, httplib::Response)>;

    class_<HttpServer>(m, "HttpServer", "Nexus HTTP Server")
    .def(init<>())
    .def("add", 
        [] (HttpServer& self, nexus::abstract::Restful& restful, object index) -> HttpServer& {
            self.add(restful, index.is_none() ? -1 : cast<int>(index));
            return self;
        },
        arg("restful"),
        arg("index") = none(),
        "Adds a Nexus restful to the server's context.",
        return_value_policy::reference_internal
    )
    .def("Get", 
        [] (HttpServer& self, const std::string& path) {
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
        [] (HttpServer& self, const std::string& path) {
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
        [] (HttpServer& self, const std::string& path) {
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
        [] (HttpServer& self, const std::string& path) {
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
        [] (HttpServer& self, const std::string& path) {
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
        [] (HttpServer& self, const std::string& path) {
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
        &HttpServer::listen,
        arg("host"),
        arg("port"),
        "Starts listening for incoming connections on a specific port.",
        return_value_policy::reference_internal
    )
    .def("stop", 
        &HttpServer::stop,
        "Stops the server and closes all connections.",
        return_value_policy::reference_internal
    )
    .def_property_readonly("is_running", 
        &HttpServer::is_running,
        "Checks if the server is currently running.",
        return_value_policy::reference_internal
    )
    .def_property_readonly("logger", 
        [] (HttpServer& self) {
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