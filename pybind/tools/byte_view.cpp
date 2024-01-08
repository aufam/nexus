#include "../pybind.h"
#include "nexus/tools/byte_view.h"
#include <future>
#include <etl/keywords.h>

using namespace std::literals;

namespace pybind11 { 
    void bindByteView(module_& m); 
}

void pybind11::bindByteView(module_& m) {
    using ByteViewIterator = etl::Iter<const uint8_t*>;
    class_<ByteViewIterator>(m, "ByteViewIterator")
    .def("__next__", 
        [] (ByteViewIterator& self) -> const uint8_t& { 
            if (not self)
                throw stop_iteration();
            
            const uint8_t& res = *self;
            ++self;
            return res; 
        }, 
        return_value_policy::reference_internal
    );

    class_<nexus::byte_view>(m, "_ByteView")
    .def("__len__", 
        &nexus::byte_view::len, 
        "Returns the number of devices in the listener."
    )
    .def("__getitem__", 
        [] (nexus::byte_view& self, int index) -> const uint8_t& {
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
        [] (nexus::byte_view& self) -> ByteViewIterator { 
            return etl::iter(self); 
        },
        "Return device iterator."
    )
    .def("to_string",
        [] (nexus::byte_view& self) -> std::string { 
            return self; 
        },
        "Convert to string."
    )
    .def("to_vector",
        [] (nexus::byte_view& self) -> std::vector<uint8_t> { 
            return self; 
        },
        "Convert to string."
    );

    class_<byte_view, nexus::byte_view>(m, "ByteView")
    .def(init<const object&>());
}
