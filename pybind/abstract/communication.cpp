#include "../pybind.h"
#include "nexus/abstract/communication.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindCommunication(module_& m); 

    class Communication : virtual public nexus::abstract::Communication {
    public:
        Communication() = default;

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Communication, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Communication, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Communication, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Communication, post, method_name, json_request);
        }
    };
}

void pybind11::bindCommunication(module_& m) {
    class_<nexus::abstract::Communication, Communication, nexus::abstract::Restful>(m, "Communication", "Abstract Communication")
    .def(init<>());
}
