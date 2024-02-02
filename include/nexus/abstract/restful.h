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

namespace Project::nexus::abstract::c_wrapper { 
    class Restful : virtual public abstract::Restful {
    public:
        Restful() = default;
        virtual ~Restful();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        char* (*c_path)(void*);
        char* (*c_json)(void*);
        char* (*c_post)(void*, const char*, const char*);
        char* (*c_patch)(void*, const char*);
        void* c_members;
    };
}

#else
/// Opaque handle representing a Nexus RESTful object.
typedef void* nexus_restful_t;

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
void nexus_restful_delete(nexus_restful_t);

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

#endif 
#endif // PROJECT_NEXUS_ABSTRACT_RESTFUL_H