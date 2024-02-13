#include "../pybind.h"
#include "nexus/abstract/restful.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindRestful(module_& m); 

    class Restful : virtual public nexus::abstract::Restful {
    public:
        Restful() = default;
        virtual ~Restful() {}

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Restful, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Restful, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Restful, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Restful, post, method_name, json_request);
        }
    };
}

void pybind11::bindRestful(module_& m) {
    class_<nexus::abstract::Restful, Restful, std::shared_ptr<nexus::abstract::Restful>>(m, "Restful", "Abstract Restful")
    .def(init<>())
    .def("path", 
        &nexus::abstract::Restful::path,
        "Retrieves the object name."
    )
    .def("json",
        &nexus::abstract::Restful::json,
        "Converts the object's data to a JSON string representation."
    )
    .def("patch",
        [] (nexus::abstract::Restful& self, std::string_view json_string) { 
            gil_scoped_release gil_release; 
            return self.patch(json_string); 
        },
        arg("json_string"),
        "Patches (updates) certain parameters on the object."
    )
    .def("post",
        [] (nexus::abstract::Restful& self, std::string_view method_name, std::string_view json_string) { 
            gil_scoped_release gil_release; 
            return self.post(method_name, json_string); 
        },
        arg("method_name"),
        arg("json_string"),
        "Posts a request to invoke a method on the object."
    );
}
