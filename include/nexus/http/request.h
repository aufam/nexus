#ifndef PROJECT_NEXUS_HTTP_REQUEST_H
#define PROJECT_NEXUS_HTTP_REQUEST_H

#ifdef __cplusplus
#include <httplib.h>
#else

typedef const void* nexus_http_request_t;

/// Retrieves the HTTP method of a request.
/// @param request Handle to the request.
/// @return The HTTP method as a string (e.g., "GET", "POST").
const char* nexus_http_request_get_method(nexus_http_request_t request);

/// Retrieves the path of a request.
/// @param request Handle to the request.
/// @return The path of the request.
const char* nexus_http_request_get_path(nexus_http_request_t request);

/// Retrieves the body of a request.
/// @param request Handle to the request.
/// @return The body of the request, or NULL if there is no body.
const char* nexus_http_request_get_body(nexus_http_request_t request);

/// Retrieves the remote address of the client that sent the request.
/// @param request Handle to the request.
/// @return The remote address as a string.
const char* nexus_http_request_get_remote_address(nexus_http_request_t request);

/// Retrieves the local address that the request was received on.
/// @param request Handle to the request.
/// @return The local address as a string.
const char* nexus_http_request_get_local_address(nexus_http_request_t request);

/// Retrieves the remote port of the client that sent the request.
/// @param request Handle to the request.
/// @return The remote port number.
int nexus_http_request_get_remote_port(nexus_http_request_t request);

/// Retrieves the local port that the request was received on.
/// @param request Handle to the request.
/// @return The local port number.
int nexus_http_request_get_local_port(nexus_http_request_t request);

/// Checks if the request has a specific header.
/// @param request Handle to the request.
/// @param key The name of the header to check.
/// @return 1 if the header exists, 0 if not.
int nexus_http_request_has_header(nexus_http_request_t request, const char* key);

/// Retrieves the value of a specific header.
/// @param request Handle to the request.
/// @param key The name of the header to retrieve.
/// @return The value of the header, or NULL if the header does not exist.
const char* nexus_http_request_get_header_value(nexus_http_request_t request, const char* key);

#endif
#endif