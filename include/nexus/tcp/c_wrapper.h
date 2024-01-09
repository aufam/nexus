#ifndef PROJECT_NEXUS_TCP_C_WRAPPER_H
#define PROJECT_NEXUS_TCP_C_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Nexus tcp server object handler
typedef void* nexus_tcp_server_t;

/// Nexus tcp client object handler
typedef void* nexus_tcp_client_t;

/// Creates a new Nexus TCP Server object.
nexus_tcp_server_t nexus_tcp_server_new();
void nexus_tcp_server_delete(nexus_tcp_server_t server);

int nexus_tcp_server_listen(nexus_tcp_server_t server, const char* host, int port);
void nexus_tcp_server_stop(nexus_tcp_server_t server);
void nexus_tcp_server_add_callback(nexus_tcp_server_t server, uint8_t* (*callback)(const uint8_t* request, size_t* length));
void nexus_tcp_server_set_logger(nexus_tcp_server_t server, 
    void (*logger)(const char* ip, const uint8_t* request, size_t request_length, const uint8_t* response, size_t response_length)
);


nexus_tcp_client_t nexus_tcp_client_new(const char* host, int port);
uint8_t* nexus_tcp_client_request(nexus_tcp_client_t client, const uint8_t* request, size_t* length, int timeout_milliseconds);
void nexus_tcp_client_delete(nexus_tcp_client_t client);

#ifdef __cplusplus
}
#endif
#endif // PROJECT_NEXUS_TCP_C_WRAPPER_H
