#include "nexus/modbus/tcp/server.h"
#include <etl/keywords.h>

using namespace nexus;

modbus::tcp::Server::Server() {
    this->addCallback([this] (nexus::byte_view buffer) -> std::vector<uint8_t> {
        val res = modbus::api::decode(buffer);
        return modbus::api::encode(callback(0xFF, res));
    });
}