#ifndef PROJECT_NEXUS_ABSTRACT_DEVICE_H
#define PROJECT_NEXUS_ABSTRACT_DEVICE_H

#ifdef __cplusplus
#include "nexus/abstract/restful.h"

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

#else
#include "nexus/abstract/c_wrapper.h"
#endif
#endif // PROJECT_NEXUS_ABSTRACT_DEVICE_H