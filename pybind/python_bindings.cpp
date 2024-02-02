#include "pybind.h"
#include <etl/keywords.h>

namespace pybind11 {
    void bindBaudRate(module_& m);
    void bindByteView(module_& m); 

    // abstract
    void bindRestful(module_& m); 
    void bindDevice(module_& m); 
    void bindCommunication(module_& m); 
    void bindListener(module_& m); 
    void bindSerial(module_& m); 
    void bindClient(module_& m); 
    void bindCodec(module_& m); 

    // serial
    void bindSerialSoftware(module_& m); 
    void bindSerialHardware(module_& m); 

    // tcp
    void bindTCPClient(module_& m); 
    void bindTCPServer(module_& m);

    // modbus
    void bindModbusAPI(module_& m); 
    void bindModbusTCPClient(module_& m); 
    void bindModbusTCPServer(module_& m);
    void bindModbusRTUClient(module_& m); 
    void bindModbusRTUServer(module_& m);

    // http
    void bindHttpRequest(module_& m);
    void bindHttpResponse(module_& m);
    void bindHttpServer(module_& m);
    void bindHttpClient(module_& m); 
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
    py::bindClient(m); 
    py::bindCodec(m); 

    py::bindSerialHardware(m); 
    py::bindSerialSoftware(m); 

    py::bindTCPClient(m); 
    py::bindTCPServer(m);

    py::bindModbusAPI(m); 
    py::bindModbusTCPClient(m); 
    py::bindModbusTCPServer(m); 
    py::bindModbusRTUClient(m); 
    py::bindModbusRTUServer(m); 
    
    py::bindHttpRequest(m);
    py::bindHttpResponse(m);
    py::bindHttpServer(m);
    py::bindHttpClient(m);
}