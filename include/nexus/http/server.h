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

        struct RestfulHandler {
            std::shared_ptr<abstract::Restful> restful; 
            std::string base_path = "";
            std::unordered_map<std::string, std::string> query = {}; 
        };
    
        Server& add(RestfulHandler args);

        /// Adds a RESTful endpoint to the server.
        /// @param restful The RESTful endpoint to be added.
        /// @param base_path Default = "".
        /// @param query map<string, string> for path query, default = {}.
        /// @return A reference to this Server object, allowing for method chaining.
        Server& add(
            std::shared_ptr<abstract::Restful> restful, 
            std::string base_path = "", 
            std::unordered_map<std::string, std::string> query = {}
        ) {
            return add(RestfulHandler{.restful=std::move(restful), .base_path=std::move(base_path), .query=std::move(query)});
        }

        Server& add(
            abstract::Restful& restful, 
            std::string base_path = "", 
            std::unordered_map<std::string, std::string> query = {}
        ) {
            return add(std::shared_ptr<abstract::Restful>(&restful, [](abstract::Restful*) {}), std::move(base_path), std::move(query));
        }
    
    protected:
        std::list<RestfulHandler> handlers;

    private:
        std::list<std::shared_ptr<abstract::Restful>> find_restful(const httplib::Request& request, bool all = false) const;
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
/// @param base_path base path.
/// @param query_keys array of query keys.
/// @param query_values array of query values.
/// @param query_len query len.
void nexus_http_server_add_restful(
    nexus_http_server_t server, 
    nexus_restful_t restful, 
    const char* base_path, 
    const char** query_keys, 
    const char** query_values, 
    size_t query_len
);

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