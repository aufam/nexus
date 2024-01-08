#include "nexus/http/client.h"
#include "nexus/http/c_wrapper.h"
#include <etl/keywords.h>

using namespace Project::nexus;

http::Client::Client(Args args) : httplib::Client(args.host, args.port) {}

fun static cast(nexus_http_client_t client) {
    return static_cast<nexus::http::Client*>(client);
}

fun nexus_http_client_new(const char* path, int port) -> nexus_http_client_t {
    return new nexus::http::Client(path, port);
}

fun nexus_http_client_delete(nexus_http_client_t client) -> void {
    delete cast(client);
}

fun nexus_http_client_GET(nexus_http_client_t client, const char* path) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Get(path))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_HEAD(nexus_http_client_t client, const char* path) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Head(path))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_POST(nexus_http_client_t client, const char* path, const char* body, const char* content_type) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Post(path, body, content_type))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_PUT(nexus_http_client_t client, const char* path, const char* body, const char* content_type) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Put(path, body, content_type))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_PATCH(nexus_http_client_t client, const char* path, const char* body, const char* content_type) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Patch(path, body, content_type))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_DELETE(nexus_http_client_t client, const char* path, const char* body, const char* content_type) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Delete(path, body, content_type))
        *res = std::move(*response.operator->());
    return res;
}

fun nexus_http_client_OPTIONS(nexus_http_client_t client, const char* path) -> nexus_http_response_t {
    var res = new httplib::Response();
    if (var response = cast(client)->Options(path))
        *res = std::move(*response.operator->());
    return res;
}