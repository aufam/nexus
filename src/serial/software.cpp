#include "nexus/serial/software.h"
#include "nexus/tools/detect_virtual_comm.h"
#include "nexus/tools/json.h"
#include <future>
#include <fcntl.h>   /* File Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions */
#include <etl/keywords.h>

using namespace nexus;
using namespace std::literals;

serial::Software::Software(std::shared_ptr<Hardware> ser) : Hardware::Interface(ser) {}
serial::Software::Software(std::shared_ptr<Hardware> ser, std::shared_ptr<abstract::Codec> codec) : Hardware::Interface(ser, codec) {}

serial::Software::Software(Args args) 
    : Hardware::Interface(std::make_shared<Hardware>(args.port, args.speed, args.timeout, args.codec)) 
{}

fun serial::Software::post(std::string_view method_name, std::string_view json_string) -> std:: string {
    val json = tools::json_parse(json_string);
    if (method_name == "reconnect") {
        val port = json["port"].to_string_or(ser_->port.c_str());
        val speed = json["speed"].to_string();
        val timeout = json["timeout"].to_int_or(ser_->timeout.count());

        ser_->port = std::string(port.data(), port.len());
        ser_->speed = speed == "B115200" ? B115200 : speed == "B9600" ? B9600 : speed == "B57600" ? B57600 : ser_->speed;
        ser_->timeout = std::chrono::milliseconds(timeout);
        reconnect();
        return tools::json_response_status_success("Request to reconnect serial communication");
    }

    return abstract::Serial::post(method_name, json_string);
}

extern "C" {
    typedef void* nexus_serial_software_t;
    typedef void* nexus_serial_hardware_t;
    typedef void* nexus_serial_hardware_callback_id_t;
    typedef void* nexus_codec_t;

    nexus_serial_software_t nexus_serial_new(nexus_serial_hardware_t ser, nexus_codec_t codec) {
        auto base = static_cast<abstract::Serial*>(ser);
        auto hw = dynamic_cast<serial::Hardware*>(base);
        auto sw = dynamic_cast<serial::Software*>(base);

        if (hw) {
            return codec 
                ? new serial::Software(std::shared_ptr<serial::Hardware>(hw), 
                    std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec))
                ) 
                : new serial::Software(std::shared_ptr<serial::Hardware>(hw));
        }

        if (sw) {
            return codec 
                ? new serial::Software(sw->getSerialHardware(), 
                    std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec))
                ) 
                : new serial::Software(sw->getSerialHardware());
        }

        return nullptr;
    }

    nexus_serial_hardware_callback_id_t nexus_serial_software_add_callback(nexus_serial_software_t ser, void (*callback)(const uint8_t* buffer, size_t length)) {
        var it = static_cast<serial::Software*>(ser)->addCallback([callback] (byte_view buffer) { callback(buffer.data(), buffer.len()); });
        var res = ::malloc(sizeof(it));
        ::memcpy(res, &it, sizeof(it));
        return res;
    }

    void nexus_serial_software_remove_callback(nexus_serial_software_t ser, nexus_serial_hardware_callback_id_t callback) {
        static_cast<serial::Software*>(ser)->removeCallback(*reinterpret_cast<serial::Hardware::CallbackId*>(callback));
        ::free(callback);
    }
}