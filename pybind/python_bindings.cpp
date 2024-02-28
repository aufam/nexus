#include "pybind.h"
#include <future>
#include <etl/keywords.h>

namespace pybind11 {
    void bindBaudRate(module_& m);
    void bindByteView(module_& m); 
    void bindContentType(module_& m); 

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
    m.attr("base_factory") = py::dict();

    py::bindByteView(m); 
    py::bindBaudRate(m);
    py::bindContentType(m);

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

    class Future {
    public:
        Future(py::function fn) : fn(std::move(fn)) {}

        Future then(py::function then_fn) {
            return py::cpp_function([captured_fn=std::move(fn), then_fn=std::move(then_fn)]() {
                py::gil_scoped_acquire acquire;
                return then_fn(captured_fn());
            });
        }

        py::object await_() {
            py::gil_scoped_acquire acquire;
            return fn();
        }

        std::future<py::object> launch() {
            return std::async([captured_fn=std::move(fn)]() mutable {
                py::gil_scoped_acquire acquire;
                return captured_fn();
            });
        }
        
        py::function fn;
    };

    m.def("async_", [](py::function func) {
        return py::cpp_function([func](py::args args, py::kwargs kwargs) {
            return Future(py::cpp_function([func, args, kwargs]() {
                py::gil_scoped_acquire acquire;
                return func(*args, **kwargs);
            }));
        });
    });

    py::class_<Future>(m, "Future")
        .def(py::init<py::function>())
        .def("then", &Future::then, py::arg("then_function"))
        .def_property_readonly("await_", &Future::await_)
        .def("launch", &Future::launch);
    
    py::enum_<std::future_status>(m, "StdFutureStatus")
        .value("READY", std::future_status::ready)
        .value("TIMEOUT", std::future_status::timeout)
        .value("DEFERRED", std::future_status::deferred);

    py::class_<std::future<py::object>>(m, "StdFuture")
        .def("get", [](std::future<py::object>& self) {
            py::gil_scoped_release release;
            self.wait(); // Wait for the future to be ready
            py::gil_scoped_acquire acquire; // Acquire GIL before accessing Python objects
            return self.get(); // Get the result
        })
        .def("valid", [](std::future<py::object>& self) {
            return self.valid();
        })
        .def("wait", [](std::future<py::object>& self) {
            py::gil_scoped_release release;
            self.wait();
        })
        .def("wait_for", [](std::future<py::object>& self, std::chrono::milliseconds timeout) {
            py::gil_scoped_release release;
            return self.wait_for(timeout);
        });
}