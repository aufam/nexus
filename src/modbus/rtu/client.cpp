#include "nexus/modbus/rtu/client.h"
#include <etl/keywords.h>

using namespace nexus;

fun modbus::rtu::Client::encode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return modbus::api::encode(buffer);
}

fun modbus::rtu::Client::decode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return modbus::api::decode(buffer);
}

fun modbus::rtu::Client::Request(nexus::byte_view buffer) -> std::vector<uint8_t> {
    this->send(buffer);
    return this->receiveBytes([addr=buffer[0], fc=buffer[1]] (nexus::byte_view buffer) -> bool {
        return buffer.len() >= 2 and buffer[0] == addr and buffer[1] == fc;
    });
}
