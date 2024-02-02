#ifndef PROJECT_NEXUS_ABSTRACT_DEVICE_H
#define PROJECT_NEXUS_ABSTRACT_DEVICE_H

#include "nexus/abstract/restful.h"

#ifdef __cplusplus
namespace Project::nexus::abstract { 

    /// Abstract class representing any device, inheriting RESTful capabilities.
    class Device : virtual public Restful {
    protected:
        /// Protected constructor to prevent direct instantiation.
        Device() = default;

    public:
        /// Virtual destructor for proper cleanup in derived classes.
        virtual ~Device() {}

        /// Updates all device parameters.
        virtual void update() {}
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Device : virtual public abstract::Device {
    public:
        Device() = default;
        virtual ~Device();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;
        void update() override;

        abstract::Restful* restful;
        void (*c_update)(void*);
        void* c_members;
    };
}
#else
/// Opaque handle representing a Nexus device object.
typedef void* nexus_device_t;

/// Create a new Device object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @param update A function pointer for updating the object's members.
/// @return A pointer to the newly created Device object.
nexus_device_t nexus_device_override_new(
    nexus_restful_t restful,
    void (*update)(void* members),
    void* members
);

/// @brief Delete a Device object with overridden functions.
/// @param device A pointer to a Restful object.
void nexus_device_delete(nexus_device_t device);

/// Updates a Nexus device object.
/// @param device Handle to the Nexus device object.
void nexus_device_update(nexus_device_t device);
#endif
#endif // PROJECT_NEXUS_ABSTRACT_DEVICE_H