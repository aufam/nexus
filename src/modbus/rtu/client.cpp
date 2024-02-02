#include "nexus/modbus/rtu/client.h"
#include <etl/keywords.h>

using namespace nexus;

modbus::rtu::Client::Client(Args args) 
    : api::Client(args.server_address)
    , serial::Hardware::Interface(std::make_shared<serial::Hardware>(args.port, args.speed, args.timeout, std::make_shared<api::Codec>())) 
{}

modbus::rtu::Client::Client(int server_address, std::shared_ptr<serial::Hardware> ser) 
    : modbus::api::Client(server_address)
    , serial::Hardware::Interface(ser, std::make_shared<api::Codec>())
{}

fun modbus::rtu::Client::request(byte_view buffer) -> byte_view {
    ser_->sendCodec(codec_, buffer);
    return ser_->receiveCodec(codec_, [this] (byte_view received_buffer) { return received_buffer[0] == server_address; });
}

modbus::rtu::Client::~Client() {}

extern "C" {
    typedef void* nexus_modbus_rtu_client_t;

    nexus_modbus_rtu_client_t nexus_modbus_rtu_client_new(int server_address, const char* port, speed_t speed, int timeout) {
        return new modbus::rtu::Client(server_address, port, speed, std::chrono::milliseconds(timeout));
    }
}