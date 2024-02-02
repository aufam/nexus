#ifndef PROJECT_NEXUS_HTTP_SERVER_H
#define PROJECT_NEXUS_HTTP_SERVER_H

#include "nexus/abstract/restful.h"
#include "nexus/http/response.h"
#include "nexus/http/request.h"

#ifdef __cplusplus

namespace Project::nexus::http { 

    /// A server class that extends the functionality of httplib::Server.
    /// This class provides additional features for managing RESTful endpoints.
    class Server : public httplib::Server {
    public:
        /// Constructs a Server object.
        /// Initializes the server with default settings.
        Server();

        /// Destructor for the Server object.
        /// Cleans up any resources associated with the server.
        virtual ~Server() {}

        /// Adds a RESTful endpoint to the server.
        /// @param restful The RESTful endpoint to be added.
        /// @param index An optional additional path segment for handling multiple
        ///              endpoints with the same base path. If non-negative, the path
        ///              is registered as "/base_path/{index}". If not specified,
        ///              only the base path is registered.
        /// @return A reference to this Server object, allowing for method chaining.
        Server& add(abstract::Restful& restful, int index = -1);
    };
}

#else

/// Opaque handle representing a Nexus HTTP server.
typedef void* nexus_http_server_t;

/// Function pointer for handling HTTP requests.
typedef void (*nexus_http_server_handler_t)(nexus_http_request_t request, nexus_http_response_t response);

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

#endif
#endif // PROJECT_NEXUS_HTTP_SERVER_H