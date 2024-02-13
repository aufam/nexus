#include "../pybind.h"
#include "nexus/abstract/listener.h"
#include <iostream>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindListener(module_& m); 

    class Listener : public nexus::abstract::Listener {
    public:
        Listener() = default;
        virtual ~Listener() {}

        std::string path() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Listener, path, );
        }
        std::string json() const override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Listener, json, );
        }
        std::string patch(std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Listener, patch, json_request);
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            PYBIND11_OVERRIDE(std::string, nexus::abstract::Listener, post, method_name, json_request);
        }
    };
}

void pybind11::bindListener(module_& m) {
    using DeviceIterator = etl::Iter<nexus::abstract::Listener::iterator<nexus::abstract::Device>>;
    class_<DeviceIterator>(m, "DeviceIterator")
    .def("__next__", 
        [] (DeviceIterator& self) -> nexus::abstract::Device& { 
            if (not self)
                throw stop_iteration();
            
            nexus::abstract::Device& res = *self;
            ++self;
            return res; 
        }, 
        return_value_policy::reference_internal
    );

    class_<nexus::abstract::Listener, Listener, nexus::abstract::Restful, std::shared_ptr<nexus::abstract::Listener>>(m, "Listener")
    .def(init<>())
    .def("stop",
        [] (nexus::abstract::Listener& self) {
            gil_scoped_release release;
            self.stop();
        },
        "Stop the thread"
    )
    .def("add", 
        &nexus::abstract::Listener::add,
        // [] (nexus::abstract::Listener& self, object device) -> nexus::abstract::Listener& {
        //     if (!isinstance<nexus::abstract::Device>(device))
        //         throw std::invalid_argument("Invalid object type: expected Device class");
        //     self.add(std::shared_ptr<nexus::abstract::Device>(device.release().cast<nexus::abstract::Device*>()));
        //     return self;
        // },
        arg("device"),
        "Adds a device to the listener."
    )
    .def("remove",
        [] (nexus::abstract::Listener& self, int index) -> nexus::abstract::Listener& {
            try {
                self.remove(index);
                return self;
            } catch (const std::out_of_range& e) {
                throw index_error(e.what());
            }
        },
        arg("index"),
        "Removes a device from the listener by index."
    )
    .def("__len__", 
        &nexus::abstract::Listener::len, 
        "Returns the number of devices in the listener."
    )
    .def("__getitem__", 
        [] (nexus::abstract::Listener& self, int index) -> nexus::abstract::Device& {
            try {
                return self[index];
            } catch (const std::out_of_range& e) {
                throw index_error(e.what());
            }
        },
        arg("index"), 
        return_value_policy::reference_internal, 
        "Reference to the device at the specified index."
    )
    .def("__iter__", 
        [] (nexus::abstract::Listener& self) -> DeviceIterator { 
            return etl::iter(self); 
        },
        "Return device iterator."
    )
    .def_readwrite("interval",
        &nexus::abstract::Listener::interval,
        "Time interval between updates."
    );
}
