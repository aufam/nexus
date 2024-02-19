#include "nexus/abstract/device.h"
#include "nexus/http/server.h"
#include "nexus/http/client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static const char* host = "localhost";
static int port = 6000;

typedef struct {
    int x;
} member_t;

static char* json(void* member) {
    char* res = (char*) malloc(32);
    sprintf(res, "{\"x\": %d}", ((member_t*) (member))->x);
    return res;
}

static char* pathA(void* member) {
    (void) member;
    char* res = (char*) malloc(32);
    sprintf(res, "/a");
    return res;
}

static char* pathB(void* member) {
    (void) member;
    char* res = (char*) malloc(32);
    sprintf(res, "/b");
    return res;
}

static void updateA(void* member) {
    ++(((member_t*) (member))->x);
}

static void updateB(void* member) {
    --(((member_t*) (member))->x);
}

static void get_test_handler(nexus_http_request_t request, nexus_http_response_t response) {
    (void) request;
    nexus_http_response_set_status(response, 200);
    nexus_http_response_set_content(response, "{\"msg\": \"echo test\"}", "application/json");
}

static void* server_listen_non_blocking(nexus_http_server_t server) {
    nexus_http_server_listen(server, host, port);
    return NULL;
}

int c_http_server() {
    member_t member_a = {.x=0};
    member_t member_b = {.x=0};

    nexus_device_t a = nexus_device_override_new(nexus_restful_override_new(pathA, json, NULL, NULL, &member_a), updateA, &member_a);
    nexus_device_t b = nexus_device_override_new(nexus_restful_override_new(pathB, json, NULL, NULL, &member_b), updateB, &member_b);

    nexus_http_server_t server = nexus_http_server_new();
    nexus_http_server_add_restful(server, a, "", NULL, NULL, 0);
    nexus_http_server_add_restful(server, b, "", NULL, NULL, 0);
    nexus_http_server_add_method(server, "GET", "/test", get_test_handler);

    pthread_t thd;
    pthread_create(&thd, NULL, server_listen_non_blocking, server);
    printf("Server is running on http://%s:%d/\n", host, port);

    usleep(1e3);
    nexus_device_update(a);
    nexus_device_update(b);

    nexus_http_client_t client = nexus_http_client_new(host, port);
    nexus_http_response_t ra = nexus_http_client_GET(client, "/a");
    nexus_http_response_t rb = nexus_http_client_GET(client, "/b");
    nexus_http_response_t rt = nexus_http_client_GET(client, "/test");

    printf("a: %s\n", nexus_http_response_get_body(ra));
    printf("b: %s\n", nexus_http_response_get_body(rb));
    printf("test: %s\n", nexus_http_response_get_body(rt));

    nexus_http_response_delete(ra);
    nexus_http_response_delete(rb);
    nexus_http_response_delete(rt);

    nexus_http_server_stop(server);
    pthread_join(thd, NULL);
    
    nexus_http_client_delete(client);
    nexus_http_server_delete(server);
    // nexus_device_delete(a);
    // nexus_device_delete(b);
    return 0;
}