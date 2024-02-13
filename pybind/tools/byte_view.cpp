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

    class_<nexus::byte_view>(m, "ByteView")
    .def(init<>())
    .def(init<std::vector<uint8_t>>())
    .def(init<std::string>())
    .def("__len__", 
        &nexus::byte_view::len, 
        "Returns the number of devices in the listener."
    )
    .def("__eq__", 
        &nexus::byte_view::operator==, 
        "Equal operator."
    )
    .def("__ne__", 
        &nexus::byte_view::operator!=, 
        "Not equal operator."
    )
    .def("__bool__", 
        [] (nexus::byte_view& self) -> bool { 
            return self.len() > 0; 
        }, 
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
    .def("__getitem__", 
        [] (nexus::byte_view& self, slice sl) -> nexus::byte_view {
            ssize_t start, stop, step, length;
            size_t size = self.size();

            if (!sl.compute(size, &start, &stop, &step, &length))
                throw std::runtime_error("Unable to compute slice");
            
            return self.slice(start, stop, step);
        },
        arg("slice"), 
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
        &nexus::byte_view::to_string,
        "Convert to string."
    )
    .def("to_vector",
        &nexus::byte_view::to_vector,
        "Convert to vector."
    );
}
