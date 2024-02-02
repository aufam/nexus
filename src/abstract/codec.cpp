#include "nexus/abstract/codec.h"
#include "nexus/tools/json.h"
#include "nexus/tools/to_string.h"
#include "helper.ipp"
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

fun abstract::Codec::post(std::string_view method_name, std::string_view json_request) -> std::string {
    val json = tools::json_parse(json_request);

    val buffer = json["buffer"];
    if (not buffer.is_list())
        return tools::json_response_status_fail_mismatch_value_type();
    
    var buf = std::vector<uint8_t>(buffer.len());
    for (var [src, dest] in etl::zip(buffer, buf))
        dest = src.to_int();

    if (method_name == "encode") {
        return tools::json_concat(tools::json_response_status_success("Encode"), "{"
            "\"res\": " + tools::to_string(encode(buf).to_vector()) +
        "}");
    }

    if (method_name == "decode") {
        return tools::json_concat(tools::json_response_status_success("Decode"), "{"
            "\"res\": " + tools::to_string(decode(buf).to_vector()) +
        "}");
    }

    return tools::json_response_status_fail_unknown_method();
}

abstract::c_wrapper::Codec::~Codec() { 
    delete restful; 
}

fun abstract::c_wrapper::Codec::path() const -> std::string { 
    return restful ? restful->path() : abstract::Codec::path(); 
}

fun abstract::c_wrapper::Codec::json() const -> std::string { 
    return restful ? restful->json() : abstract::Codec::json(); 
}

fun abstract::c_wrapper::Codec::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return restful ? restful->post(method_name, json_request) : abstract::Codec::post(method_name, json_request);
}

fun abstract::c_wrapper::Codec::patch(std::string_view json_request) -> std::string {
    return restful ? restful->patch(json_request) : abstract::Codec::patch(json_request);
}

nexus::byte_view abstract::c_wrapper::Codec::encode(nexus::byte_view buffer) const { 
    size_t length = buffer.len();
    uint8_t* res = c_encode ?  c_encode(c_members, buffer.data(), &length) : nullptr; 
    etl::ScopeExit se([res] { ::free(res); });
    return std::move(std::vector(res, res + length));
}

nexus::byte_view abstract::c_wrapper::Codec::decode(nexus::byte_view buffer) const { 
    size_t length = buffer.len();
    uint8_t* res = c_decode ?  c_decode(c_members, buffer.data(), &length) : nullptr; 
    etl::ScopeExit se([res] { ::free(res); });
    return std::move(std::vector(res, res + length));
}

extern "C" {
    typedef void* nexus_restful_t;
    typedef void* nexus_codec_t;

    fun static cast(nexus_codec_t codec) { 
        return static_cast<abstract::Codec*>(codec); 
    }

    nexus_codec_t nexus_codec_override_new(
        nexus_restful_t restful,
        uint8_t* (*encode)(void* members, const uint8_t* buffer, size_t* length),
        uint8_t* (*decode)(void* members, const uint8_t* buffer, size_t* length),
        void* members
    ) {
        var res = new abstract::c_wrapper::Codec();
        res->restful = static_cast<abstract::c_wrapper::Restful*>(restful);
        res->c_encode = encode;
        res->c_decode = decode;
        res->c_members = members;
        return res;
    }

    void nexus_codec_delete(nexus_codec_t codec) {
        delete cast(codec);
    }

    uint8_t* nexus_codec_encode(nexus_codec_t codec, const uint8_t* buffer, size_t* length) {
        var res = cast(codec)->encode(nexus::byte_view{buffer, *length});
        *length = res.len();

        uint8_t* ret = (uint8_t*) ::malloc(*length);
        ::memcpy(ret, res.data(), *length);
        return ret;
    }

    uint8_t* nexus_codec_decode(nexus_codec_t codec, const uint8_t* buffer, size_t* length) {
        var res = cast(codec)->decode(nexus::byte_view{buffer, *length});
        *length = res.len();
        
        uint8_t* ret = (uint8_t*) ::malloc(*length);
        ::memcpy(ret, res.data(), *length);
        return ret;
    }
}