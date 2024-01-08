#include "../pybind.h"
#include "nexus/abstract/listener.h"
#include <iostream>
#include <future>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindListener(module_& m); 

    class Listener : public nexus::abstract::Listener {
    public:
        Listener() = default;
        virtual ~Listener() {}

        std::string path() const override {
            return _path ? _path() : nexus::abstract::Listener::path();
        }
        std::string json() const override {
            return _json ? _json() : nexus::abstract::Listener::json();
        }
        std::string post(std::string_view method_name, std::string_view json_request) override {
            return _post ? _post(method_name, json_request) : nexus::abstract::Listener::post(method_name, json_request);
        }
        std::string patch(std::string_view json_request) override {
            return _patch ? _patch(json_request) : nexus::abstract::Listener::patch(json_request);
        }

        std::function<std::string()> _path;
        std::function<std::string()> _json;
        std::function<std::string(std::string_view, std::string_view)> _post;
        std::function<std::string(std::string_view)> _patch;
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

    class_<Listener, nexus::abstract::Restful>(m, "Listener")
    .def(init<>())
    .def("stop",
        &Listener::stop,
        "Stop the thread"
    )
    .def("__del__",
        [] (Listener& self) {
            gil_scoped_release release;
            self.stop();
        }
    )
    .def("add", 
        [] (Listener& self, object device) -> Listener& {
            if (!isinstance<nexus::abstract::Device>(device))
                throw std::invalid_argument("Invalid object type: expected Device class");
            self.add(std::unique_ptr<nexus::abstract::Device>(device.release().cast<nexus::abstract::Device*>()));
            return self;
        },
        arg("device"),
        "Adds a device to the listener."
    )
    .def("remove",
        [] (Listener& self, int index) -> Listener& {
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
        &Listener::len, 
        "Returns the number of devices in the listener."
    )
    .def("__getitem__", 
        [] (Listener& self, int index) -> nexus::abstract::Device& {
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
        [] (Listener& self) -> DeviceIterator { 
            return etl::iter(self); 
        },
        "Return device iterator."
    )
    .def_readwrite("interval",
        &Listener::interval,
        "Time interval between updates."
    )
    .def_property_readonly("path_override",
        [] (Listener& self) {
            return std::function ([&self] (const std::function<std::string()>& path) {
                self._path = path;
            });
        },
        "Override path as function decorator"
    )
    .def_property_readonly("json_override",
        [] (Listener& self) {
            return std::function ([&self] (const std::function<std::string()>& json) {
                self._json = json;
            });
        },
        "Override json as function decorator"
    )
    .def_property_readonly("post_override",
        [] (Listener& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view, std::string_view)>& post) {
                self._post = post;
            });
        },
        "Override post as function decorator"
    )
    .def_property_readonly("patch_override",
        [] (Listener& self) {
            return std::function ([&self] (const std::function<std::string(std::string_view)>& patch) {
                self._patch = patch;
            });
        },
        "Override patch as function decorator"
    );
}
