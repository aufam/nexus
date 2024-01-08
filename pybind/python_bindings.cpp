#include "pybind.h"
#include <etl/keywords.h>

namespace pybind11 {
    void bindBaudRate(module_& m);
    void bindByteView(module_& m); 

    void bindRestful(module_& m); 
    void bindDevice(module_& m); 
    void bindCommunication(module_& m); 
    void bindListener(module_& m); 
    void bindSerial(module_& m); 

    void bindRequest(module_& m);
    void bindResponse(module_& m);
    void bindServer(module_& m);
    void bindClient(module_& m);

    void bindTCPClient(module_& m); 
    void bindTCPServer(module_& m);

    void bindModbusTCPClient(module_& m); 
    void bindModbusTCPServer(module_& m); 
    void bindModbusAPI(module_& m); 
}

namespace py = pybind11;

PYBIND11_MODULE(py_nexus, m) {
    m.doc() = "Python bindings for Nexus library";
    m.attr("__version__") = NEXUS_VERSION;

    py::bindByteView(m); 
    py::bindBaudRate(m);

    py::bindRestful(m); 
    py::bindDevice(m); 
    py::bindCommunication(m); 
    py::bindListener(m); 
    py::bindSerial(m); 
    
    py::bindRequest(m);
    py::bindResponse(m);
    py::bindServer(m);
    py::bindClient(m);

    py::bindTCPClient(m); 
    py::bindTCPServer(m);

    py::bindModbusTCPClient(m); 
    py::bindModbusTCPServer(m); 
    py::bindModbusAPI(m); 
}