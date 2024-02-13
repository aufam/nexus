#include "../pybind.h"
#include "nexus/abstract/serial.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindSerial(module_& m); 

    class Serial : virtual public nexus::abstract::Serial {
    public:
        Serial() = default;

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Serial, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Serial, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Serial, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Serial, post, method_name, json_request);
        }

        void reconnect() override {
            PYBIND11_OVERRIDE(void, nexus::abstract::Serial, reconnect, );
        }
        void disconnect() override {
            PYBIND11_OVERRIDE(void, nexus::abstract::Serial, disconnect, );
        }
        bool isConnected() const override {
            PYBIND11_OVERRIDE(bool, nexus::abstract::Serial, isConnected, );
        }

        int send(nexus::byte_view buffer) override { 
            PYBIND11_OVERRIDE(int, nexus::abstract::Serial, send, buffer);
        }
        nexus::byte_view receive() override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Serial, receive, );
        }
        nexus::byte_view receive(std::function<bool(nexus::byte_view)> filter) override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Serial, receive, std::move(filter));
        }
    };
}

void pybind11::bindSerial(module_& m) {
    class_<nexus::abstract::Serial, Serial, nexus::abstract::Restful, std::shared_ptr<nexus::abstract::Serial>>(m, 
        "Serial", 
        "Abstract Asynchronous Serial class"
    )
    .def(init<>())
    .def("send", 
        &nexus::abstract::Serial::send,
        arg("buffer"),
        "Sends buffer over the serial connection. Returns: Number of bytes sent, or -1 if failed."
    )
    .def("receive",
        [] (nexus::abstract::Serial& self) { 
            gil_scoped_release gil_release; 
            return self.receive(); 
        },
        "Receives bytes over the serial connection."
    )
    .def("receive",
        [] (nexus::abstract::Serial& self, std::function<bool(nexus::byte_view)> filter) { 
            gil_scoped_release gil_release; 
            return self.receive(std::move(filter)); 
        },
        arg("filter"),
        "Receives bytes over the serial connection."
    );
}
