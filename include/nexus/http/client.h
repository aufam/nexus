#ifndef PROJECT_NEXUS_HTTP_CLIENT_H
#define PROJECT_NEXUS_HTTP_CLIENT_H

#include "nexus/http/request.h"
#include "nexus/http/response.h"

#ifdef __cplusplus

namespace Project::nexus::http { 

    /// A client class for HTTP communication, extending httplib::Client and implementing abstract::Communication.
    /// This class provides a structured way to connect to HTTP servers and send requests.
    class Client : public httplib::Client {
    public:
        /// Arguments for constructing a Client object.
        struct Args {
            /// Host address to connect to
            std::string host;
            /// Port number to connect to
            int port;
        };

        /// Arguments for constructing a Client object.
        struct Args2 {
            /// Host address and port number to connect to
            std::string host_port;
        };

        /// Constructs a Client object with the specified connection settings.
        /// @param args The connection settings to use.
        explicit Client(Args args);
        explicit Client(Args2 args);

        /// Constructs a Client object with the specified host and port.
        /// @param host The host address to connect to.
        /// @param port The port number to connect to.
        Client(std::string host, int port) : Client(Args{.host=host, .port=port}) {}
        explicit Client(std::string host_port) : Client(Args2{.host_port=host_port}) {}

        /// Destructor for the Client object.
        /// Cleans up any resources associated with the client.
        virtual ~Client() {}
    };

}

#else
typedef void* nexus_http_client_t;

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
#endif
#endif // PROJECT_NEXUS_HTTP_CLIENT_H