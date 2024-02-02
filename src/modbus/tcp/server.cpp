#include "nexus/modbus/tcp/server.h"
#include <etl/keywords.h>

using namespace nexus;

modbus::tcp::Server::Server() : modbus::api::Server(0xFF) {
    this->addCallback([this] (byte_view buffer) -> std::vector<uint8_t> {
        val codec = modbus::api::Codec();
        val res = codec.decode(buffer);
        var ret = process_callback(res);
        return codec.encode(ret);
    });
}

extern "C" {
    typedef void* nexus_modbus_tcp_server_t;

    nexus_modbus_tcp_server_t nexus_modbus_tcp_server_new() {
        return new modbus::tcp::Server();
    }

    void nexus_modbus_tcp_server_listen(nexus_modbus_tcp_server_t server, const char* host, int port) {
        static_cast<modbus::tcp::Server*>(server)->listen(host, port);
    }

    void nexus_modbus_tcp_server_stop(nexus_modbus_tcp_server_t server) {
        static_cast<modbus::tcp::Server*>(server)->stop();
    }

    int nexus_modbus_tcp_server_is_running(nexus_modbus_tcp_server_t server) {
        return static_cast<modbus::tcp::Server*>(server)->isRunning();
    }
}