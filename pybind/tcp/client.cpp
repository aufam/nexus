#include "../pybind.h"
#include "nexus/tcp/client.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 { 
    void bindTCPClient(module_& m); 

    class TCPClient : public nexus::tcp::Client {
    public:
        TCPClient(std::string host, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000))
            : nexus::tcp::Client(host, port, timeout) {}

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::tcp::Client, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::tcp::Client, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::tcp::Client, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::tcp::Client, post, method_name, json_request);
        }

        void reconnect() override {
            PYBIND11_OVERRIDE(void, nexus::tcp::Client, reconnect, );
        }
        void disconnect() override {
            PYBIND11_OVERRIDE(void, nexus::tcp::Client, disconnect, );
        }
        bool isConnected() const override {
            PYBIND11_OVERRIDE(bool, nexus::tcp::Client, isConnected, );
        }

        nexus::byte_view request(nexus::byte_view buffer) override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::tcp::Client, request, buffer);
        }
    };
}

void pybind11::bindTCPClient(module_& m) {
    class_<nexus::tcp::Client, TCPClient, nexus::abstract::Client>(m, "TCPClient", "TCP Client")
    .def(init<std::string, int, std::chrono::milliseconds>(), 
        arg("host"), 
        arg("port"),
        arg("timeout") = std::chrono::milliseconds(1000)
    );
}