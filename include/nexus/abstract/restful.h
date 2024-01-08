#ifndef PROJECT_NEXUS_ABSTRACT_RESTFUL_H
#define PROJECT_NEXUS_ABSTRACT_RESTFUL_H

#ifdef __cplusplus
#include <string>

namespace Project::nexus::abstract { 

    /// Abstract class defining a RESTful scheme.
    class Restful {
    protected:
        /// Protected constructor to prevent direct instantiation.
        Restful() = default;

    public:
        /// Virtual destructor for proper cleanup in derived classes.
        virtual ~Restful() {}

        /// Retrieves the object name.
        /// @return The object name as a string.
        virtual std::string path() const { return {}; }

        /// Converts the object's data to a JSON string representation.
        /// @return The JSON string representing the object's data.
        virtual std::string json() const { return {}; }

        /// Posts a request to invoke a method on the object.
        /// @param method_name The name of the method to invoke.
        /// @param json_request The JSON-formatted request data.
        /// @return The JSON-formatted response from the object.
        virtual std::string post(std::string_view method_name, std::string_view json_request) { 
            (void) method_name; (void) json_request;
            return {}; 
        }

        /// Patches (updates) certain parameters on the object.
        /// @param json_request The JSON-formatted request data containing the parameters to update.
        /// @return The JSON-formatted response from the object.
        virtual std::string patch(std::string_view json_request) { 
            (void) json_request;
            return {}; 
        }
    };
}

#else
#include "nexus/abstract/c_wrapper.h"
#endif 
#endif // PROJECT_NEXUS_ABSTRACT_RESTFUL_H