#include "nexus/http/request.h"
#include <etl/keywords.h>

extern "C" {
typedef const void* nexus_http_request_t;

fun static cast_request(nexus_http_request_t request) {
    return static_cast<const httplib::Request*>(request);
}

fun nexus_http_request_get_method(nexus_http_request_t request) -> const char* {
    return cast_request(request)->method.c_str();
}

fun nexus_http_request_get_path(nexus_http_request_t request) -> const char* {
    return cast_request(request)->path.c_str();
}

fun nexus_http_request_get_body(nexus_http_request_t request) -> const char* {
    return cast_request(request)->body.c_str();
}

fun nexus_http_request_get_remote_address(nexus_http_request_t request) -> const char* {
    return cast_request(request)->remote_addr.c_str();
}

fun nexus_http_request_get_local_address(nexus_http_request_t request) -> const char* {
    return cast_request(request)->local_addr.c_str();
}

fun nexus_http_request_get_remote_port(nexus_http_request_t request) -> int {
    return cast_request(request)->remote_port;
}

fun nexus_http_request_get_local_port(nexus_http_request_t request) -> int {
    return cast_request(request)->local_port;
}

fun nexus_http_request_has_header(nexus_http_request_t request, const char* key) -> int {
    return cast_request(request)->has_header(key);
}

fun nexus_http_request_get_header_value(nexus_http_request_t request, const char* key) -> const char* {
    return cast_request(request)->get_header_value(key).c_str();
}
}