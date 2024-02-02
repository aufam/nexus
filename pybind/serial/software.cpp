#include "../pybind.h"
#include "nexus/serial/software.h"
#include <future>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindSerialSoftware(module_& m); 

    class SerialSoftware : public nexus::serial::Software {
    public:
        SerialSoftware(
            std::string port, 
            speed_t speed, 
            std::chrono::milliseconds timeout, 
            std::shared_ptr<nexus::abstract::Codec> codec
        ) : nexus::serial::Software(port, speed, timeout, codec) {}
        
        SerialSoftware(
            std::string port, 
            speed_t speed, 
            std::chrono::milliseconds timeout=Default::timeout
        ) : nexus::serial::Software(port, speed, timeout, nullptr) {}

        SerialSoftware(std::shared_ptr<nexus::serial::Hardware> ser, std::shared_ptr<nexus::abstract::Codec> codec)
            : nexus::serial::Software(ser, codec) {}

        SerialSoftware(std::shared_ptr<nexus::serial::Hardware> ser)
            : nexus::serial::Software(ser) {}

        SerialSoftware() : nexus::serial::Software() {}

        virtual ~SerialSoftware() {}

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::serial::Software, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::serial::Software, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::serial::Software, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::serial::Software, post, method_name, json_request);
        }

        void reconnect() override {
            PYBIND11_OVERRIDE(void, nexus::serial::Software, reconnect, );
        }
        void disconnect() override {
            PYBIND11_OVERRIDE(void, nexus::serial::Software, disconnect, );
        }
        bool isConnected() const override {
            PYBIND11_OVERRIDE(bool, nexus::serial::Software, isConnected, );
        }

        int send(nexus::byte_view buffer) override { 
            PYBIND11_OVERRIDE(int, nexus::serial::Software, send, buffer);
        }
        nexus::byte_view receive() override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Serial, receive, );
        }
        nexus::byte_view receive(std::function<bool(nexus::byte_view)> filter) override { 
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::serial::Software, receive, std::move(filter));
        }
    };
}

void pybind11::bindSerialSoftware(module_& m) {
    class_<nexus::serial::Software, SerialSoftware, nexus::abstract::Serial, nexus::serial::Hardware::Interface>(m, "SerialSoftware", "Software Serial Communication")
    .def(init<>())
    .def(init<std::string, speed_t, std::chrono::milliseconds, std::shared_ptr<nexus::abstract::Codec>>(),
        arg("port"), 
        arg("speed"),
        arg("timeout") = SerialSoftware::Default::timeout,
        arg("codec")
    )
    .def(init<std::string, speed_t, std::chrono::milliseconds, std::shared_ptr<nexus::abstract::Codec>>(),
        arg("port"), 
        arg("speed"),
        arg("timeout"),
        arg("codec")
    )
    .def(init<std::shared_ptr<nexus::serial::Hardware>, std::shared_ptr<nexus::abstract::Codec>>(),
        arg("hardware_serial"),
        arg("codec")
    );
}
