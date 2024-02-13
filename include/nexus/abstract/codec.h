#ifndef PROJECT_NEXUS_ABSTRACT_CODEC_H
#define PROJECT_NEXUS_ABSTRACT_CODEC_H

#include "nexus/abstract/restful.h"

#ifdef __cplusplus
#include "nexus/tools/byte_view.h"

namespace Project::nexus::abstract { 

    /// Abstract class representing any codec, inheriting RESTful capabilities.
    class Codec : virtual public Restful {
    protected:
        Codec() = default;

    public:
        virtual ~Codec() {}

        // Restful post

        /// Restful post
        /// @param method_name Method name: "reconnect", "disconnect"
        /// @param json_request Request as json string. Format: {"buffer": <list[int]>}
        /// @return Response as json string. Format: {"res": <list[int]>}
        std::string post(std::string_view method_name, std::string_view json_request) override;

        /// Encode raw buffer.
        virtual nexus::byte_view encode(nexus::byte_view buffer) const { return buffer; };

        /// Decode raw buffer.
        virtual nexus::byte_view decode(nexus::byte_view buffer) const { return buffer; };
    };
}

namespace Project::nexus::abstract::c_wrapper { 
    class Codec : virtual public abstract::Codec {
    public:
        Codec() = default;
        virtual ~Codec();

        std::string path() const override;
        std::string json() const override;
        std::string post(std::string_view method_name, std::string_view json_request) override;
        std::string patch(std::string_view json_request) override;

        nexus::byte_view encode(nexus::byte_view buffer) const override;
        nexus::byte_view decode(nexus::byte_view buffer) const override;

        abstract::Restful* restful;
        uint8_t* (*c_encode)(void*, const uint8_t*, size_t*);
        uint8_t* (*c_decode)(void*, const uint8_t*, size_t*);
        void* c_members;
    };
}
#else
#include <stdint.h>
#include <stddef.h>

/// Opaque handle representing a Nexus codec object.
typedef void* nexus_codec_t;

/// Create a new codec object with overridden functions.
/// @param restful A pointer to the Restful override.
/// @param update A function pointer for updating the object's members.
/// @return A pointer to the newly created codec object.
nexus_codec_t nexus_codec_override_new(
    nexus_restful_t restful,
    uint8_t* (*encode)(void* members, const uint8_t* buffer, size_t* length),
    uint8_t* (*decode)(void* members, const uint8_t* buffer, size_t* length),
    void* members;
);

/// @brief Delete a codec object with overridden functions.
/// @param codec A pointer to a Restful object.
void nexus_codec_delete(nexus_codec_t codec);

uint8_t* nexus_codec_encode(nexus_codec_t codec, const uint8_t* buffer, size_t* length);
uint8_t* nexus_codec_decode(nexus_codec_t codec, const uint8_t* buffer, size_t* length);

#endif
#endif // PROJECT_NEXUS_ABSTRACT_CODEC_H