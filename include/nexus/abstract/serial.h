#ifndef PROJECT_NEXUS_ABSTRACT_SERIAL_H
#define PROJECT_NEXUS_ABSTRACT_SERIAL_H

#include "nexus/abstract/communication.h"

#ifdef __cplusplus
#include "nexus/tools/byte_view.h"
#include <chrono>
#include <functional>

namespace Project::nexus::abstract { 

    /// Abstract class representing any device, inheriting RESTful capabilities.
    class Serial : virtual public Communication {
    protected:
        /// Protected constructor to prevent direct instantiation.
        Serial() = default;

    public:
        /// Virtual destructor for proper cleanup in derived classes.
        virtual ~Serial() {}

        std::string path() const override { return "/serial"; }
        std::string post(std::string_view method_name, std::string_view json_request) override;

        /// Sends buffer over the serial connection.
        /// @param buffer Buffer to send.
        /// @return Number of bytes sent, or -1 if failed.
        virtual int send(byte_view buffer) { (void) buffer; return -1; };

        /// Receives bytes over the serial connection.
        /// @return A vector of received bytes.
        virtual byte_view receive() { return receive([] (byte_view) { return true; }); };

        /// Receives bytes over the serial connection with filter.
        /// @param filter Filter receive
        /// @return A vector of received bytes.
        virtual byte_view receive(std::function<bool(byte_view)> filter) { (void) filter; return {}; };
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Serial : virtual public abstract::Serial {
    public:
        Serial() = default;
        virtual ~Serial();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        void reconnect() override;
        void disconnect() override;
        bool isConnected() const override;

        int send(byte_view buffer) override;
        byte_view receive() override;
        byte_view receive(std::function<bool(byte_view)> filter) override;

        abstract::Communication* communication;
        int (*c_send)(void*, const uint8_t*, size_t);
        uint8_t* (*c_receive)(void*, size_t*);
        void* c_members;
    };
}

#else

/// Opaque handle representing a Nexus serial object.
typedef void* nexus_serial_t;

/// Create a new Serial object with overridden functions.
/// @param communication A pointer to the Communication override
/// @return A pointer to the newly created Listener object.
nexus_serial_t nexus_serial_override_new(
    nexus_communication_t communication,
    int (*send)(void* members, const uint8_t* buffer, size_t length),
    uint8_t* (*receive)(void* members, size_t* length),
    void* members
);

/// @brief Delete a Serial object.
/// @param serial A pointer to a Serial object.
void nexus_serial_delete(nexus_serial_t serial);

int nexus_serial_send(nexus_serial_t serial, const uint8_t* buffer, size_t length);
uint8_t* nexus_serial_receive(nexus_serial_t serial, size_t* length);
uint8_t* nexus_serial_receive_filter(nexus_serial_t serial, size_t* length, int (*filter) (const uint8_t* buffer, size_t length));

#endif
#endif // PROJECT_NEXUS_ABSTRACT_SERIAL_H