#include "nexus/tcp/c_wrapper.h"
#include "nexus/tcp/server.h"
#include "nexus/tcp/client.h"
#include <etl/keywords.h>

extern "C" {
    fun static cast_server(nexus_tcp_server_t server) {
        return static_cast<nexus::tcp::Server*>(server);
    }

    fun static cast_client(nexus_tcp_client_t client) {
        return static_cast<nexus::tcp::Client*>(client);
    }

    fun nexus_tcp_server_new() -> nexus_tcp_server_t {
        return new nexus::tcp::Server();
    }

    int nexus_tcp_server_listen(nexus_tcp_server_t server, const char* host, int port) {
        return cast_server(server)->listen(host, port);
    }

    void nexus_tcp_server_stop(nexus_tcp_server_t server) {
        cast_server(server)->stop();
    }

    void nexus_tcp_server_add_callback(nexus_tcp_server_t server, uint8_t* (*callback)(const uint8_t* request, size_t* length)) {
        cast_server(server)->addCallback([callback] (nexus::byte_view bv) -> std::vector<uint8_t> {
            var req = bv.data();
            var len = bv.len();
            var res = callback(req, &len);
            var ret = std::vector<uint8_t>(res, res + len);
            ::free(res);
            return ret;
        });
    }

    void nexus_tcp_server_set_logger(
        nexus_tcp_server_t server, 
        void (*logger)(const char*, const uint8_t*, size_t, const uint8_t*, size_t)
    ) {
        cast_server(server)->setLogger([logger] (const char* ip, nexus::byte_view request, nexus::byte_view response) {
            logger(ip, request.data(), request.len(), response.data(), response.len());
        });
    }

    void nexus_tcp_server_delete(nexus_tcp_server_t server) {
        delete cast_server(server);
    }

    nexus_tcp_client_t nexus_tcp_client_new(const char* host, int port) {
        return new nexus::tcp::Client(host, port);
    }

    uint8_t* nexus_tcp_client_request(nexus_tcp_client_t client, const uint8_t* request, size_t* length, int timeout_milliseconds) {
        val [res, err] = cast_client(client)->request({request, *length}, std::chrono::milliseconds(timeout_milliseconds));
        if (err != nexus::tcp::Client::Error::NONE) {
            *length = 0;
            return nullptr;
        }
        
        var ret = new uint8_t[res.size()];
        ::memcpy(ret, res.data(), res.size());
        *length = res.size();
        return ret;
    }

    void nexus_tcp_client_delete(nexus_tcp_client_t client) {
        delete cast_client(client);
    }
}



