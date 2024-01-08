#ifndef PROJECT_NEXUS_HTTP_C_WRAPPER_H
#define PROJECT_NEXUS_HTTP_C_WRAPPER_H

#include <stdint.h>
#include <stddef.h>
#include "nexus/c_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif


/// Opaque handle representing a Nexus HTTP server.
typedef void* nexus_http_server_t;

/// Opaque handle representing a Nexus HTTP client.
typedef void* nexus_http_client_t;

/// Opaque handle representing a Nexus HTTP request.
typedef const void* nexus_http_request_t;

/// Opaque handle representing a Nexus HTTP response.
typedef void* nexus_http_response_t;

/// Function pointer for handling HTTP requests.
typedef void (*nexus_http_server_handler_t)(nexus_http_request_t request, nexus_http_response_t response);

/// @defgroup server HTTP Server Functions
/// Functions for creating and managing HTTP servers.
/// @{

/// Creates a new HTTP server.
/// @return Handle to the created server, or NULL on error.
nexus_http_server_t nexus_http_server_new();

/// Deletes an HTTP server.
/// @param server Handle to the server to delete.
void nexus_http_server_delete(nexus_http_server_t server);

/// Adds a handler for a specific HTTP method and path.
/// @param server Handle to the server.
/// @param path The path to match.
/// @param method The HTTP method (e.g., "GET", "POST").
/// @param handler The function to call for matching requests.
void nexus_http_server_add_method(nexus_http_server_t server, const char* method, const char* path, nexus_http_server_handler_t handler);

/// Adds a Nexus restful to the server's context.
/// @param server Handle to the server.
/// @param restful Handle to the restful object.
void nexus_http_server_add_restful(nexus_http_server_t server, nexus_restful_t restful);

/// Adds a Nexus device with a custom index to the server's context.
/// @param server Handle to the server.
/// @param restful Handle to the restful object.
void nexus_http_server_add_restful_with_index(nexus_http_server_t server, nexus_restful_t restful, int index);

/// Starts listening for incoming connections on a specific port.
/// @param server Handle to the server.
/// @param host Host name or IP address to bind to (NULL for all interfaces).
/// @param port Port number to listen on.
void nexus_http_server_listen(nexus_http_server_t server, const char* host, int port);

/// Checks if the server is currently running.
/// @param server Handle to the server.
/// @return 1 if running, 0 if not.
int nexus_http_server_is_running(nexus_http_server_t server);

/// Stops the server and closes all connections.
/// @param server Handle to the server.
void nexus_http_server_stop(nexus_http_server_t server);

/// Set logger function
/// @param server Handle to the server.
void nexus_http_server_set_logger(nexus_http_server_t server, nexus_http_server_handler_t handler);

/// @}

/// @defgroup server HTTP Client Functions
/// Functions for creating and managing HTTP clients.
/// @{

/// Creates a new HTTP client.
/// @param host Host name or IP address of the server to connect to.
/// @param port Port number of the server to connect to.
/// @return Handle to the created client, or NULL on error.
nexus_http_client_t nexus_http_client_new(const char* host, int port);

/// Deletes an HTTP client.
/// @param client Handle to the client to delete.
void nexus_http_client_delete(nexus_http_client_t client);

/// Sends an HTTP GET request and returns the response.
/// @param client Handle to the client.
/// @param path The path to request.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_GET(nexus_http_client_t client, const char* path);

/// Sends an HTTP HEAD request and returns the response headers.
/// @param client Handle to the client.
/// @param path The path to request.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_HEAD(nexus_http_client_t client, const char* path);

/// Sends an HTTP POST request with the specified body.
/// @param client Handle to the client.
/// @param path The path to request.
/// @param body The body of the request.
/// @param content_type The content type of the request body.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_POST(nexus_http_client_t client, const char* path, const char* body, const char* content_type);

/// Sends an HTTP PUT request with the specified body.
/// @param client Handle to the client.
/// @param path The path to request.
/// @param body The body of the request.
/// @param content_type The content type of the request body.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_PUT(nexus_http_client_t client, const char* path, const char* body, const char* content_type);

/// Sends an HTTP PATCH request with the specified body.
/// @param client Handle to the client.
/// @param path The path to request.
/// @param body The body of the request.
/// @param content_type The content type of the request body.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_PATCH(nexus_http_client_t client, const char* path, const char* body, const char* content_type);

/// Sends an HTTP DELETE request.
/// @param client Handle to the client.
/// @param path The path to request.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_DELETE(nexus_http_client_t client, const char* path, const char* body, const char* content_type);

/// Sends an HTTP OPTIONS request and returns the allowed methods.
/// @param client Handle to the client.
/// @param path The path to request.
/// @return Handle to the received response, or NULL on error.
nexus_http_response_t nexus_http_client_OPTIONS(nexus_http_client_t client, const char* path);

/// @}

/// @defgroup request HTTP Request Functions
/// @brief Functions for accessing information about HTTP requests.
/// @{

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

/// @}
/// @defgroup response HTTP Response Functions
/// @brief Functions for creating and manipulating HTTP responses.
/// @{

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

/// @}


#ifdef __cplusplus
}
#endif
#endif