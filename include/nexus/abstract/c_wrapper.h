#ifndef PROJECT_NEXUS_ABSTRACT_C_WRAPPER_H
#define PROJECT_NEXUS_ABSTRACT_C_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Opaque handle representing a Nexus RESTful object.
typedef void* nexus_restful_t;

/// Opaque handle representing a Nexus device object.
typedef void* nexus_device_t;

/// Opaque handle representing a Nexus communication object.
typedef void* nexus_communication_t;

/// Opaque handle representing a Nexus listener object.
typedef void* nexus_listener_t;

/// @defgroup C wrapper for Nexus Restful
/// @brief Functions for creating and manipulating Nexus Restful in C.
/// @{

/// Create a new Restful object with overridden functions.
/// @param json A function pointer for the JSON generation.
/// @param path A function pointer for the path retrieval.
/// @param members A pointer to additional data.
/// @return A pointer to the newly created Restful object.
nexus_restful_t nexus_restful_override_new(
    char* (*path)(void* members), 
    char* (*json)(void* members), 
    char* (*post)(void* members, const char* method_name, const char* json_request),
    char* (*patch)(void* members, const char* json_request),
    void* members
);

/// @brief Delete a Restful object with overridden functions.
/// @param restful A pointer to a Restful object.
void nexus_restful_override_delete(nexus_restful_t);

/// Retrieves the path of a RESTful object.
/// @param restful Handle to the Nexus RESTful object.
/// @return Pointer to the path string.
char* nexus_restful_path(nexus_restful_t restful);

/// Retrieves the JSON representation of a RESTful object.
/// @param restful Handle to the Nexus RESTful object.
/// @return Pointer to the JSON string.
char* nexus_restful_json(nexus_restful_t restful);

/// Sends a POST request to a RESTful object.
/// @param restful Handle to the Nexus RESTful object.
/// @param method_name Name of the method to call.
/// @param json_request JSON request data.
/// @return Pointer to the response JSON string.
char* nexus_restful_post(nexus_restful_t restful, const char* method_name, const char* json_request);

/// Sends a PATCH request to a RESTful object.
/// @param restful Handle to the Nexus RESTful object.
/// @param json_request JSON request data.
/// @return Pointer to the response JSON string.
char* nexus_restful_patch(nexus_restful_t restful, const char* json_request);

/// @}
/// @defgroup C wrapper for Nexus Device
/// @brief Functions for creating and manipulating Nexus Device in C.
/// @{

/// Create a new Device object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @param update A function pointer for updating the object's members.
/// @return A pointer to the newly created Device object.
nexus_device_t nexus_device_override_new(
    nexus_restful_t restful,
    void (*update)(void* members)
);

/// @brief Delete a Device object with overridden functions.
/// @param device A pointer to a Restful object.
void nexus_device_override_delete(nexus_device_t device);

/// Updates a Nexus device object.
/// @param device Handle to the Nexus device object.
void nexus_device_update(nexus_device_t device);

/// @}
/// @defgroup C wrapper for Nexus Communication
/// @brief Functions for creating and manipulating Nexus Communication in C.
/// @{

/// Create a new Communication object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @return A pointer to the newly created Communication object.
nexus_communication_t nexus_communication_override_new(
    nexus_restful_t restful
);

/// @brief Delete a Communication object with overridden functions.
/// @param communication A pointer to a Restful object.
void nexus_communication_override_delete(nexus_communication_t communication);

/// @}
/// @defgroup C wrapper for Nexus Listener
/// @brief Functions for creating and manipulating Nexus Listener in C.
/// @{

/// Create a new Listener object.
/// @return A pointer to the newly created Listener object.
nexus_listener_t nexus_listener_new();

/// @brief Delete a Listener object.
/// @param listener A pointer to a Listener object.
void nexus_listener_delete(nexus_listener_t listener);

/// Create a new Listener object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @return A pointer to the newly created Listener object.
nexus_listener_t nexus_listener_override_new(
    nexus_restful_t restful
);

/// @brief Delete a Listener object with overridden functions.
/// @param listener A pointer to a Listener object.
void nexus_listener_override_delete(nexus_listener_t listener);

/// Adds a device to a listener.
/// @param listener Handle to the Nexus listener object.
/// @param device Handle to the Nexus device object to add.
void nexus_listener_add(nexus_device_t listener, nexus_device_t device);

/// Removes a device from a listener.
/// @param listener Handle to the Nexus listener object.
/// @param index Index of the device to remove.
void nexus_listener_remove(nexus_device_t listener, int index);

/// Retrieves the number of devices in a listener.
/// @param listener Handle to the Nexus listener object.
/// @return Number of devices in the listener.
size_t nexus_listener_len(nexus_device_t listener);

/// Retrieves a device from a listener by index.
/// @param listener Handle to the Nexus listener object.
/// @param index Index of the device to retrieve.
/// @return Handle to the device, or NULL if not found.
nexus_device_t nexus_listener_get_device(nexus_device_t listener, int index);

/// @}

#ifdef __cplusplus
}

#include "nexus/abstract/restful.h"
#include "nexus/abstract/device.h"
#include "nexus/abstract/communication.h"
#include "nexus/abstract/listener.h"

namespace Project::nexus::cwrapper {
    class Restful : virtual public nexus::abstract::Restful {
    public:
        Restful() = default;
        virtual ~Restful();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        char* (*cpath)(void*);
        char* (*cjson)(void*);
        char* (*cpost)(void*, const char*, const char*);
        char* (*cpatch)(void*, const char*);
        void* cmembers;
    };

    class Device : virtual public nexus::abstract::Device {
    public:
        Device() = default;
        virtual ~Device();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;
        void update() override;

        cwrapper::Restful* restful;
        void (*cupdate)(void*);
    };

    class Communication : virtual public nexus::abstract::Communication {
    public:
        Communication() = default;
        virtual ~Communication();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        cwrapper::Restful* restful;
    };

    class Listener : virtual public nexus::abstract::Listener {
    public:
        Listener() = default;
        virtual ~Listener();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        cwrapper::Restful* restful;
    };
}
#endif
#endif // PROJECT_NEXUS_ABSTRACT_C_WRAPPER_H
