#ifndef PROJECT_NEXUS_MODBUS_API_H
#define PROJECT_NEXUS_MODBUS_API_H

#include "nexus/abstract/codec.h"
#include "nexus/abstract/client.h"

#ifdef __cplusplus

namespace Project::nexus::modbus {
    enum Error {
        NONE,
        TIMEOUT,
        DATA_FRAME,
    };

    enum FunctionCode {
        READ_COILS = 1,
        READ_DISCRETE_INPUTS = 2,
        READ_HOLDING_REGISTERS = 3,
        READ_INPUT_REGISTERS = 4,
        WRITE_SINGLE_COIL = 5,
        WRITE_SINGLE_REGISTER = 6,
        READ_EXCEPTION_STATUS = 7,
        DIAGNOSTIC = 8,
        WRITE_MULTIPLE_COILS = 15,
        WRITE_MULTIPLE_REGISTERS = 16,
    };
}

namespace Project::nexus::modbus::api {
    class Codec : virtual public abstract::Codec {
    public:
        std::string path() const override { return "/modbus_codec"; }
        nexus::byte_view encode(nexus::byte_view buffer) const override;
        nexus::byte_view decode(nexus::byte_view buffer) const override;
    };
    
    uint16_t crc(nexus::byte_view buffer);
}

#else
uint8_t* nexus_modbus_encode(const uint8_t* buffer, size_t* length);
uint8_t* nexus_modbus_decode(const uint8_t* buffer, size_t* length);

uint16_t nexus_modbus_crc(const uint8_t* buffer, size_t length);
#endif
#endif // PROJECT_NEXUS_MODBUS_API_H