#include "nexus/http/response.h"
#include <etl/keywords.h>

extern "C" {

typedef void* nexus_http_response_t;

fun static cast(nexus_http_response_t response) {
    return static_cast<httplib::Response*>(response);
}

fun nexus_http_response_get_version(nexus_http_response_t response) -> const char* {
    return cast(response)->version.c_str();
}

fun nexus_http_response_get_status(nexus_http_response_t response) -> int {
    return cast(response)->status;
}

fun nexus_http_response_get_reason(nexus_http_response_t response) -> const char* {
    return cast(response)->reason.c_str();
}

fun nexus_http_response_get_body(nexus_http_response_t response) -> const char* {
    return cast(response)->body.c_str();
}

fun nexus_http_response_get_location(nexus_http_response_t response) -> const char* {
    return cast(response)->location.c_str();
}

fun nexus_http_response_has_header(nexus_http_response_t response, const char* key) -> int {
    return cast(response)->has_header(key);
}

fun nexus_http_response_get_header_value(nexus_http_response_t response, const char* key) -> const char* {
    return cast(response)->get_header_value(key).c_str();
}

fun nexus_http_response_set_status(nexus_http_response_t response, int status) -> void {
    cast(response)->status = status;
}

fun nexus_http_response_set_content(nexus_http_response_t response, const char* body, const char* content_type) -> void {
    cast(response)->set_content(body, content_type);
}

fun nexus_http_response_delete(nexus_http_response_t response) -> void {
    delete cast(response);
}

}
