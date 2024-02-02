#ifndef PROJECT_NEXUS_ABSTRACT_CLIENT_H
#define PROJECT_NEXUS_ABSTRACT_CLIENT_H

#include "nexus/abstract/communication.h"

#ifdef __cplusplus
#include "nexus/tools/byte_view.h"
#include <chrono>

namespace Project::nexus::abstract { 

    /// Abstract class representing any device, inheriting RESTful capabilities.
    class Client : virtual public Communication {
    protected:
        /// Protected constructor to prevent direct instantiation.
        Client() = default;

    public:
        /// Virtual destructor for proper cleanup in derived classes.
        virtual ~Client() {}

        std::string path() const override { return "/client"; }
        std::string post(std::string_view method_name, std::string_view json_request) override;

        /// Sends buffer over the Client connection.
        /// @param buffer Buffer to send.
        /// @return Number of bytes sent, or -1 if failed.
        virtual nexus::byte_view request(nexus::byte_view buffer) { return buffer; };
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Client : virtual public abstract::Client {
    public:
        Client() = default;
        virtual ~Client();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        void reconnect() override;
        void disconnect() override;
        bool isConnected() const override;

        nexus::byte_view request(nexus::byte_view buffer) override;

        abstract::Communication* communication;
        uint8_t* (*c_request)(void*, const uint8_t*, size_t*);
        void* c_members;
    };
}

#else
#include <stdint.h>
#include <stddef.h>

/// Opaque handle representing a Nexus client object.
typedef void* nexus_client_t;

/// Create a new Client object with overridden functions.
/// @param communication A pointer to the Communication override
/// @return A pointer to the newly created Listener object.
nexus_client_t nexus_client_override_new(
    nexus_communication_t communication,
    uint8_t* (*request)(void* members, const uint8_t* buffer, size_t* length),
    void* members
);

/// @brief Delete a Client object with overridden functions.
/// @param client A pointer to a Client object.
void nexus_client_delete(nexus_client_t client);

uint8_t* nexus_client_request(nexus_client_t client, const uint8_t* buffer, size_t* length);

#endif
#endif // PROJECT_NEXUS_ABSTRACT_CLIENT_H