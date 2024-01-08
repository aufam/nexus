#ifndef PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H
#define PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H

#ifdef __cplusplus
#include "nexus/abstract/restful.h"

namespace Project::nexus::abstract { 

    /// Abstract class representing a communication interface for hardware interaction,
    /// inheriting RESTful capabilities.
    class Communication : virtual public Restful {
    protected:
        /// Protected constructor to prevent direct instantiation.
        /// Subclasses should provide concrete implementations for hardware communication.
        Communication() = default;

    public:
        /// Disabled copy constructor to prevent unintended object copies.
        Communication(const Communication&) = delete;

        /// Disabled copy assignment operator to prevent unintended object assignments.
        Communication& operator=(const Communication&) = delete;

        /// Virtual destructor for proper cleanup in derived classes.
        virtual ~Communication() {}
    };
}

#else
#include "nexus/abstract/c_wrapper.h"
#endif
#endif // PROJECT_NEXUS_ABSTRACT_COMMUNICATION_H