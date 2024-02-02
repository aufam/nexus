#include "nexus/tcp/server.h"
#include "nexus/tcp/client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static const char* host = "127.0.0.1";
static int port = 6004;

static uint8_t* callback(const uint8_t* request, size_t* length) {
    uint8_t* response = (uint8_t*) malloc(*length);
    memcpy(response, request, *length);
    return response;
}

static void logger(const char *ip, const uint8_t *request, size_t request_length, const uint8_t *response, size_t response_length) {
    printf("IP: %s, Req: %.*s, Res: %.*s\n", ip, (int) request_length, (const char*) request, (int) response_length, (const char*) response);
}

static void* server_listen_non_blocking(nexus_tcp_server_t server) {
    nexus_tcp_server_listen(server, host, port);
    return NULL;
}

int c_tcp_server() {
    nexus_tcp_server_t server = nexus_tcp_server_new();
    nexus_tcp_server_add_callback(server, callback);
    nexus_tcp_server_set_logger(server, logger);

    pthread_t thd;
    pthread_create(&thd, NULL, server_listen_non_blocking, server);
    printf("Server is running on http://%s:%d/\n", host, port);

    usleep(1e3);

    nexus_tcp_client_t client = nexus_tcp_client_new(host, port, 1000);
    const char req[] = "test";
    size_t length = sizeof(req);
    uint8_t* res = nexus_client_request(client, (const uint8_t*) req, &length);
    printf("Response: %.*s\n", (int) length, (const char*) res);
    printf("length: %d\n", (int) length);
    free(res);

    nexus_tcp_server_stop(server);
    pthread_join(thd, NULL);
    
    nexus_client_delete(client);
    nexus_tcp_server_delete(server);
    return 0;
}