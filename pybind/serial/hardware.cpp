#include "../pybind.h"
#include "nexus/serial/software.h"
#include <future>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindSerialHardware(module_& m);
}

void pybind11::bindSerialHardware(module_& m) {
    class_<nexus::serial::Hardware, nexus::abstract::Serial>(m, "SerialHardware", "Serial Hardware Communication")
    .def(init<std::string, speed_t, std::chrono::milliseconds, std::shared_ptr<nexus::abstract::Codec>>(),
        arg("port"), 
        arg("speed"),
        arg("timeout"),
        arg("codec")
    )
    .def("sendCodec",
        &nexus::serial::Hardware::sendCodec,
        arg("codec"),
        arg("buffer")
    )
    .def("receiveCodec",
        [] (nexus::serial::Hardware& self, std::shared_ptr<nexus::abstract::Codec> codec, std::function<bool(nexus::byte_view)> filter) {
            gil_scoped_release gil_release;
            return self.receiveCodec(codec, std::move(filter));
        },
        arg("codec"),
        arg("filter")
    )
    .def("addCodec",
        &nexus::serial::Hardware::addCodec,
        arg("codec")
    )
    .def("removeCodec",
        &nexus::serial::Hardware::removeCodec,
        arg("codec")
    )
    .def("addCallback",
        &nexus::serial::Hardware::addCallback,
        arg("callback")
    )
    .def("removeCallback",
        &nexus::serial::Hardware::removeCallback,
        arg("callback")
    )
    .def_readwrite("port", &nexus::serial::Hardware::port)
    .def_readwrite("speed", &nexus::serial::Hardware::speed)
    .def_readwrite("timeout", &nexus::serial::Hardware::timeout);

    class_<nexus::serial::Hardware::Interface>(m, "SerialHardwareInterface", "Serial Hardware Interface")
    .def(init<std::shared_ptr<nexus::serial::Hardware>>())
    .def(init<std::shared_ptr<nexus::serial::Hardware>, std::shared_ptr<nexus::abstract::Codec>>())
    .def("getSerialHardware", &nexus::serial::Hardware::Interface::getSerialHardware)
    .def("getCodec", &nexus::serial::Hardware::Interface::getCodec);
}
