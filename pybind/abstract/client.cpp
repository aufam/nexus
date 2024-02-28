#include "../pybind.h"
#include "nexus/abstract/client.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindClient(module_& m); 

    class Client : virtual public nexus::abstract::Client {
    public:
        Client() = default;

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Client, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Client, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Client, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Client, post, method_name, json_request);
        }

        void reconnect() override {
            PYBIND11_OVERRIDE(void, nexus::abstract::Client, reconnect, );
        }
        void disconnect() override {
            PYBIND11_OVERRIDE(void, nexus::abstract::Client, disconnect, );
        }
        bool isConnected() const override {
            PYBIND11_OVERRIDE(bool, nexus::abstract::Client, isConnected, );
        }

        nexus::byte_view request(nexus::byte_view buffer) override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Client, request, buffer);
        }
    };
}

void pybind11::bindClient(module_& m) {
    class_<nexus::abstract::Client, Client, nexus::abstract::Communication, std::shared_ptr<nexus::abstract::Client>>(m, "Client", "Abstract Client")
    .def(init<>())
    .def("request", 
        [] (nexus::abstract::Client& self, nexus::byte_view buffer) {
            gil_scoped_release gil_release;
            return self.request(buffer);
        }, 
        arg("buffer")
    );
}
