#include "../pybind.h"
#include "nexus/modbus/api.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindModbusAPI(module_& m); 
}

void pybind11::bindModbusAPI(module_& m) {
    enum_<nexus::modbus::Error>(m, "ModbusError")
    .value("NONE", nexus::modbus::Error::NONE)
    .value("TIMEOUT", nexus::modbus::Error::TIMEOUT)
    .value("DATA_FRAME", nexus::modbus::Error::DATA_FRAME)
    .export_values();

    m.def("ModbusEncode", 
        [] (object buffer) { 
            return nexus::modbus::api::encode(byte_view(buffer)); 
        }, 
        arg("buffer")
    );
    m.def("ModbusDecode", 
        [] (object buffer) { 
            return nexus::modbus::api::decode(byte_view(buffer)); 
        }, 
        arg("buffer")
    );
    m.def("ModbusCRC", 
        [] (object buffer) { 
            return nexus::modbus::api::crc(byte_view(buffer)); 
        }, 
        arg("buffer")
    );
}