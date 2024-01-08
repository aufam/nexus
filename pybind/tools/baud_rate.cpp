#include "../pybind.h"

namespace pybind11 { 
    void bindBaudRate(module_& m); 
}

void pybind11::bindBaudRate(module_& m) {
    class_<BaudRate>(m, "BaudRate")
    .def(init<int>());

    m.attr("B115200") = BaudRate(B115200);
    m.attr("B57600") = BaudRate(B57600);
    m.attr("B9600") = BaudRate(B9600);    
}
