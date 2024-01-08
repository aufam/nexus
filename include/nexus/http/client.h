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
        /// Default connection settings.
        struct Default {
            /// Default host address.
            static constexpr const char* host = "localhost";
            /// Default port number.
            static constexpr int port = 5000;
        };

        /// Arguments for constructing a Client object.
        struct Args {
            /// Host address to connect to. Defaults to `Client::Default::host`.
            std::string host = Default::host;
            /// Port number to connect to. Defaults to `Client::Default::port`.
            int port = Default::port;
        };

        /// Constructs a Client object with the specified connection settings.
        /// @param args The connection settings to use.
        explicit Client(Args args);

        /// Constructs a Client object with the specified host and port.
        /// @param host The host address to connect to.
        /// @param port The port number to connect to.
        explicit Client(std::string host, int port) : Client({.host=host, .port=port}) {}

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