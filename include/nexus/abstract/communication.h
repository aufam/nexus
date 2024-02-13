#ifndef PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H
#define PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H

#include "nexus/abstract/restful.h"

#ifdef __cplusplus
namespace Project::nexus::abstract { 

    /// Abstract class representing a communication interface.
    ///
    /// Endpoints:
    ///  * `/`:
    ///     * Method: GET
    ///     * Response: {"isConnected": <bool>} 
    ///  * `/reconnect`:
    ///     * Method: POST
    ///     * Request: {}.
    ///     * Response: JSON object with `"status": <string>` indicating the outcome of the reconnection attempt.
    ///  * `/disconnect`:
    ///     * Method: POST
    ///     * Request: Empty JSON object (`{}`).
    ///     * Response: JSON object with `"status": <string>` indicating the outcome of the disconnection attempt.
    class Communication : virtual public Restful {
    protected:
        Communication() = default;

    public:
        virtual ~Communication() {}

        /// RESTful GET
        /// @return Response as json string. Format: {"isConnected": <bool>}
        std::string json() const override;

        /// RESTful POST
        /// @param method_name Method name: "reconnect", "disconnect"
        /// @param json_request Request as json string. Format: {}
        /// @return Response as json string. Format: {"status": <string>}
        std::string post(std::string_view method_name, std::string_view json_request) override;

        virtual void reconnect() {};

        virtual void disconnect() {};
        
        virtual bool isConnected() const { return false; };
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Communication : virtual public abstract::Communication {
    public:
        Communication() = default;
        virtual ~Communication();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        void reconnect() override;
        void disconnect() override;
        bool isConnected() const override;

        abstract::Restful* restful;
        void (*c_reconnect)(void*);
        void (*c_disconnect)(void*);
        int (*c_isConnected)(void*);
        void* c_members;
    };
}
#else
/// Opaque handle representing a Nexus communication object.
typedef void* nexus_communication_t;

/// Create a new Communication object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @return A pointer to the newly created Communication object.
nexus_communication_t nexus_communication_override_new(
    nexus_restful_t restful,
    void (*reconnect)(void* members),
    void (*disconnect)(void* members),
    int (*isConnected)(void* members),
    void* members
);

/// @brief Delete a Communication object with overridden functions.
/// @param communication A pointer to a Restful object.
void nexus_communication_delete(nexus_communication_t communication);

void nexus_communication_reconnect(nexus_communication_t communication);
void nexus_communication_disconnect(nexus_communication_t communication);
int nexus_communication_is_connected(nexus_communication_t communication);

#endif
#endif // PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H