#include "../pybind.h"
#include "nexus/abstract/codec.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindCodec(module_& m); 

    class Codec : virtual public nexus::abstract::Codec {
    public:
        Codec() = default;
        virtual ~Codec() {}

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Codec, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Codec, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Codec, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Codec, post, method_name, json_request);
        }

        nexus::byte_view encode(nexus::byte_view buffer) const override {
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Codec, encode, buffer);
        }
        nexus::byte_view decode(nexus::byte_view buffer) const override {
            PYBIND11_OVERRIDE(nexus::byte_view, nexus::abstract::Codec, decode, buffer);
        }
    };
}

void pybind11::bindCodec(module_& m) {
    class_<nexus::abstract::Codec, Codec, nexus::abstract::Restful>(m, "Codec", "Abstract Codec")
    .def(init<>())
    .def("encode", &nexus::abstract::Codec::encode, arg("buffer"))
    .def("decode", &nexus::abstract::Codec::decode, arg("buffer"));
}
