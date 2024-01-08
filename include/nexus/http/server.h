#ifndef PROJECT_NEXUS_HTTP_SERVER_H
#define PROJECT_NEXUS_HTTP_SERVER_H

#ifdef __cplusplus
#include "nexus/abstract/restful.h"
#include "nexus/http/http.h"

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
#include "nexus/http/c_wrapper.h"
#endif
#endif // PROJECT_NEXUS_HTTP_SERVER_H