#include "nexus/modbus/tcp/client.h"
#include <etl/keywords.h>

using namespace nexus;

fun modbus::tcp::Client::request(byte_view buffer) -> byte_view {
    val codec = api::Codec();
    var res = cli.request(codec.encode(buffer));
    return codec.decode(res);
}

extern "C" {
    typedef void* nexus_modbus_tcp_client_t;

    nexus_modbus_tcp_client_t nexus_modbus_tcp_client_new(const char* host, int port) {
        return new modbus::tcp::Client(host, port);
    }
}