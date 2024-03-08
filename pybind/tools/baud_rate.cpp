#include "../pybind.h"

namespace pybind11 { 
    void bindBaudRate(module_& m); 
}

void pybind11::bindBaudRate(module_& m) {
    m.attr("B115200") = B115200;
    m.attr("B57600") = B57600;
    m.attr("B38400") = B38400;
    m.attr("B19200") = B19200;
    m.attr("B9600") = B9600;
    m.attr("B4800") = B4800;
    m.attr("B2400") = B2400;

    m.def("BaudRate", [] (int value) {
        switch (value) {
            case 115200: return B115200;
            case 57600:  return B57600;
            case 38400:  return B38400;
            case 19200:  return B19200;
            case 9600:   return B9600;
            case 4800:   return B4800;
            case 2400:   return B2400;
            default:     return B115200;
        }
    });
}
