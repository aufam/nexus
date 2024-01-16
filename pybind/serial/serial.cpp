#include "../pybind.h"
#include "nexus/serial/serial.h"
#include <future>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindSerial(module_& m); 

    class Serial : public nexus::serial::Serial {
    public:
        Serial(std::string port, speed_t speed, std::chrono::milliseconds timeout) : nexus::serial::Serial(port, speed, timeout) {}
        virtual ~Serial() {}

        std::string path() const override {
            return _path ? _path() : nexus::serial::Serial::path();
        }
        std::string json() const override {
            return _json ? _json() : nexus::serial::Serial::json();
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            return _post ? _post(method_name, json_request) : nexus::serial::Serial::post(method_name, json_request);
        }
        std::string patch(std::string_view json_request) override {
            return _patch ? _patch(json_request) : nexus::serial::Serial::patch(json_request);
        }
    
        std::vector<uint8_t> encode(nexus::byte_view buf) const override {
            return _encode ? _encode(buf) : nexus::serial::Serial::encode(buf);
        }
        std::vector<uint8_t> decode(nexus::byte_view buf) const override {
            return _decode ? _decode(buf) : nexus::serial::Serial::decode(buf);
        }

        std::function<std::string()> _path;
        std::function<std::string()> _json;
        std::function<std::string(std::string_view, std::string_view)> _post;
        std::function<std::string(std::string_view)> _patch;

        std::function<std::vector<uint8_t>(nexus::byte_view)> _encode;
        std::function<std::vector<uint8_t>(nexus::byte_view)> _decode;
    };
}

void pybind11::bindSerial(module_& m) {
    class_<Serial, nexus::abstract::Communication>(m, "Serial", "Hardware Serial Communication")
    .def(init<std::string, BaudRate, std::chrono::milliseconds>(), 
        arg("port"), 
        arg("speed"),
        arg("timeout") = Serial::Default::timeout
    )
    .def("isConnected",
        &Serial::isConnected,
        "Check if serial communication is connected."
    )
    .def("reconnect",
        overload_cast<>(&Serial::reconnect),
        "Reconnect serial communication."
    )
    .def("reconnect",
        lambda (Serial& self, std::string port, BaudRate speed) {
            self.reconnect({port, speed_t(speed)});
        },
        arg("port") = "auto", 
        arg("speed") = BaudRate(B115200),
        "Reconnect serial communication."
    )
    .def("disconnect",
        &Serial::disconnect,
        "Disconnect serial communication."
    )
    .def("encode",
        &Serial::encode,
        arg("bytes"),
        "Encode the raw bytes."
    )
    .def("decode",
        &Serial::decode,
        arg("bytes"),
        "Decode the raw bytes."
    )
    .def("send",
        overload_cast<nexus::byte_view>(&Serial::send),
        arg("buffer"),
        "Sends buffer over the serial connection."
    )
    .def("send",
        overload_cast<std::string_view>(&Serial::send),
        arg("text"),
        "Sends text over the serial connection."
    )
    .def("receiveBytes",
        [] (Serial& self, const std::function<bool(nexus::byte_view)>& filter, std::chrono::milliseconds timeout) {
            gil_scoped_release gil_release;
            return self.receiveBytes(filter, timeout);
        },
        arg("filter"),
        arg("timeout") = 1000ms,
        "Receives raw bytes over the serial connection, applying a filter function. Blocks until bytes matching the filter are received or the specified timeout occurs."
    )
    .def("receiveBytes",
        [] (Serial& self, std::chrono::milliseconds timeout) {
            gil_scoped_release gil_release;
            return self.receiveBytes(timeout);
        },
        arg("timeout") = 1000ms,
        "Receives raw bytes over the serial connection. Blocks until bytes are received or the specified timeout occurs."
    )
    .def("receiveText",
        [] (Serial& self, const std::function<bool(std::string_view)>& filter, std::chrono::milliseconds timeout) {
            gil_scoped_release gil_release;
            return self.receiveText(filter, timeout);
        },
        arg("filter"),
        arg("timeout") = 1000ms,
        "Receives text over the serial connection, applying a filter function. Blocks until text matching the filter is received or the specified timeout occurs."
    )
    .def("receiveText",
        [] (Serial& self, std::chrono::milliseconds timeout) {
            gil_scoped_release gil_release;
            return self.receiveText(timeout);
        },
        arg("timeout") = 1000ms,
        "Receives text over the serial connection. Blocks until text is received or the specified timeout occurs."
    )
    .def_property_readonly("callback",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<void(const std::vector<uint8_t>)>& callback) {
                self.addCallback(callback);
            });
        },
        "Adds a callback function to be invoked when data is received."
    )
    .def_property_readonly("path_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::string()>& path) {
                self._path = path;
            });
        },
        "Override path as function decorator"
    )
    .def_property_readonly("json_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::string()>& json) {
                self._json = json;
            });
        },
        "Override json as function decorator"
    )
    .def_property_readonly("post_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view, std::string_view)>& post) {
                self._post = post;
            });
        },
        "Override post as function decorator"
    )
    .def_property_readonly("patch_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view)>& patch) {
                self._patch = patch;
            });
        },
        "Override patch as function decorator"
    )
    .def_property_readonly("encode_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::vector<uint8_t>(nexus::byte_view)>& encode) {
                self._encode = encode;
            });
        },
        "Override encode as function decorator"
    )
    .def_property_readonly("decode_override",
        [] (Serial& self) {
            return std::function ([&self] (const std::function<std::vector<uint8_t>(nexus::byte_view)>& decode) {
                self._decode = decode;
            });
        },
        "Override decode as function decorator"
    )
    ;
}
