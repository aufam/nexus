#include "nexus/serial/serial.h"
#include "nexus/serial/c_wrapper.h"
#include "nexus/abstract/c_wrapper.h"
#include "../abstract/helper.ipp"
#include <etl/keywords.h>

namespace Project::nexus::cwrapper {
    class Serial : virtual public nexus::serial::Serial {
    public:
        using nexus::serial::Serial::Serial;
        virtual ~Serial() { delete restful; }

        std::string path() const { 
            auto res = get_restful_response(restful->cpath, restful->cmembers); 
            if (res.empty()) res = nexus::serial::Serial::path();
            return res;
        }

        std::string json() const { 
            auto res = get_restful_response(restful->cjson, restful->cmembers); 
            if (res.empty()) res = nexus::serial::Serial::json();
            return res;
        }

        std::string post(std::string_view method_name, std::string_view json_request) {
            auto res = get_restful_response(restful->cpost, restful->cmembers, method_name.data(), json_request.data());
            if (res.empty()) res = nexus::serial::Serial::post(method_name, json_request);
            return res;
        }

        std::string patch(std::string_view json_request) {
            auto res = get_restful_response(restful->cpatch, restful->cmembers, json_request.data());
            if (res.empty()) res = nexus::serial::Serial::patch(json_request);
            return res;
        }

        std::vector<uint8_t> encode(nexus::byte_view buffer) const override {
            if (not encoder) {
                return nexus::serial::Serial::encode(buffer);
            }

            size_t length = buffer.len();
            uint8_t* encoded = encoder(restful->cmembers, buffer.data(), &length);
            auto res = std::vector<uint8_t>(encoded, encoded + length);
            ::free(encoded);
            return res;
        }

        std::vector<uint8_t> decode(nexus::byte_view buffer) const override {
            if (not decoder) {
                return nexus::serial::Serial::decode(buffer);
            }

            size_t length = buffer.len();
            uint8_t* decoded = decoder(restful->cmembers, buffer.data(), &length);
            auto res = std::vector<uint8_t>(decoded, decoded + length);
            ::free(decoded);
            return res;
        }

        nexus_serial_codec_function_t encoder;
        nexus_serial_codec_function_t decoder;
        cwrapper::Restful* restful;
    };
}

using namespace nexus;

extern "C" {
    fun static cast(nexus_serial_t serial) {
        return static_cast<serial::Serial*>(serial);
    }

    fun nexus_serial_new(const char* port, speed_t speed) -> nexus_serial_t {
        return new serial::Serial(serial::Serial::Args{.port=port, .speed=speed});
    }

    fun nexus_serial_delete(nexus_serial_t serial) -> void {
        delete cast(serial);
    }

    fun nexus_serial_reconnect(nexus_serial_t serial, const char* port, speed_t speed) -> void {
        cast(serial)->reconnect(serial::Serial::Args{.port=port, .speed=speed});
    }

    fun nexus_serial_disconnect(nexus_serial_t serial) -> void {
        cast(serial)->disconnect();
    }

    fun nexus_serial_is_connected(nexus_serial_t serial) -> int {
        return cast(serial)->isConnected();
    }

    fun nexus_serial_send_text(nexus_serial_t serial, const char* text) -> int {
        return cast(serial)->send(text);
    }

    fun nexus_serial_receive_text(nexus_serial_t serial, int timeout) -> char* {
        val msg = cast(serial)->receiveText(std::chrono::milliseconds(timeout));
        var res = (char*) ::malloc(msg.size() + 1);
        ::strncpy(res, msg.data(), msg.size());
        return res;
    }

    fun nexus_serial_send_bytes(nexus_serial_t serial, uint8_t *data, size_t length) -> int {
        return cast(serial)->send({data, length});
    }

    fun nexus_serial_receive_bytes(nexus_serial_t serial, int timeout, size_t* length) -> uint8_t* {
        val msg = cast(serial)->receiveBytes(std::chrono::milliseconds(timeout));
        var res = (uint8_t*) ::malloc(msg.size());
        ::memcpy(res, msg.data(), msg.size());
        if (length) *length = msg.size();
        return res;
    }

    fun nexus_serial_add_callback(nexus_serial_t serial, void (*fn)(const uint8_t* data, size_t len)) -> void {
        cast(serial)->addCallback([fn] (nexus::byte_view buf) { return fn(buf.data(), buf.size()); });
    }

    fun nexus_serial_override_new(
        nexus_restful_t restful, 
        nexus_serial_codec_function_t encoder,
        nexus_serial_codec_function_t decoder,
        const char* port, speed_t speed
    ) -> nexus_serial_t {
        auto res = new nexus::cwrapper::Serial(serial::Serial::Args{.port=port, .speed=speed});
        res->restful = static_cast<nexus::cwrapper::Restful*>(restful);
        res->encoder = encoder;
        res->decoder = decoder;
        return res;
    }

    fun nexus_serial_override_delete(nexus_serial_t serial) -> void {
        delete static_cast<nexus::cwrapper::Serial*>(serial);
    }
}