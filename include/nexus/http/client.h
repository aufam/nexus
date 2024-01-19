#ifndef PROJECT_NEXUS_HTTP_CLIENT_H
#define PROJECT_NEXUS_HTTP_CLIENT_H

#ifdef __cplusplus
#include "nexus/abstract/communication.h"
#include "nexus/http/http.h"

namespace Project::nexus::http { 

    /// A client class for HTTP communication, extending httplib::Client and implementing abstract::Communication.
    /// This class provides a structured way to connect to HTTP servers and send requests.
    class Client : public httplib::Client, virtual public abstract::Communication {
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

        /// Returns the path associated with this client.
        /// @return The path "/client".
        std::string path() const override { return "/client"; };
    };

}

#else
#include "nexus/http/c_wrapper.h"
#endif
#endif // PROJECT_NEXUS_HTTP_CLIENT_H