#include "nexus/modbus/rtu/server.h"
#include <etl/keywords.h>

using namespace nexus;

modbus::rtu::Server::Server(Args args) 
    : serial::Serial(serial::Serial::Args{.port=args.port, .speed=args.speed, .timeout=args.timeout})
    , server_address(args.server_address) {
    addCallback([this] (nexus::byte_view buffer) {
        send(callback(server_address, buffer));
    });
}

fun modbus::rtu::Server::encode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return modbus::api::encode(buffer);
}

fun modbus::rtu::Server::decode(nexus::byte_view buffer) const -> std::vector<uint8_t> {
    return modbus::api::decode(buffer);
}
