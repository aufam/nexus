#ifndef PROJECT_NEXUS_HTTP_RESPONSE_H
#define PROJECT_NEXUS_HTTP_RESPONSE_H

#ifdef __cplusplus
#include <httplib.h>
#else

typedef void* nexus_http_response_t;

/// Retrieves the HTTP version of a response.
/// @param response Handle to the response.
/// @return The HTTP version as a string (e.g., "HTTP/1.1").
const char* nexus_http_response_get_version(nexus_http_response_t response);

/// Retrieves the status code of a response.
/// @param response Handle to the response.
/// @return The status code as an integer (e.g., 200 for OK).
int nexus_http_response_get_status(nexus_http_response_t response);

/// Retrieves the reason phrase of a response.
/// @param response Handle to the response.
/// @return The reason phrase as a string (e.g., "OK").
const char* nexus_http_response_get_reason(nexus_http_response_t response);

/// Retrieves the body of a response.
/// @param response Handle to the response.
/// @return The body of the response, or NULL if there is no body.
const char* nexus_http_response_get_body(nexus_http_response_t response);

/// Retrieves the Location header of a response.
/// @param response Handle to the response.
/// @return The value of the Location header, or NULL if the header does not exist.
const char* nexus_http_response_get_location(nexus_http_response_t response);

/// Checks if the response has a specific header.
/// @param response Handle to the response.
/// @param key The name of the header to check.
/// @return 1 if the header exists, 0 if not.
int nexus_http_response_has_header(nexus_http_response_t response, const char* key);

/// Retrieves the value of a specific header.
/// @param response Handle to the response.
/// @param key The name of the header to retrieve.
/// @return The value of the header, or NULL if the header does not exist.
const char* nexus_http_response_get_header_value(nexus_http_response_t response, const char* key);

/// Sets the status code of a response.
/// @param response Handle to the response.
/// @param status The status code to set.
void nexus_http_response_set_status(nexus_http_response_t response, int status);

/// Sets the body and content type of a response.
/// @param response Handle to the response.
/// @param body The body of the response.
/// @param content_type The content type of the response body.
void nexus_http_response_set_content(nexus_http_response_t response, const char* body, const char* content_type);

/// Delete response object.
/// @param response Handle to the response.
void nexus_http_response_delete(nexus_http_response_t response);

#endif
#endif