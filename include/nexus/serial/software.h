#ifndef PROJECT_NEXUS_SERIAL_SERIAL_H
#define PROJECT_NEXUS_SERIAL_SERIAL_H

#include "nexus/serial/hardware.h"

#ifdef __cplusplus

namespace Project::nexus::serial {

    /// Concrete implementation of a communication interface for hardware serial communication.
    class Software : virtual public abstract::Serial, public Hardware::Interface {
    public:
        using Hardware::Interface::Interface;

        struct Default {
            static constexpr const char* port = "auto"; 
            static constexpr speed_t speed = B115200;
            static constexpr std::chrono::milliseconds timeout = std::chrono::milliseconds(100);
        };

        /// Arguments for constructing a `Serial` object.
        struct Args { 
            std::string port=Default::port; 
            speed_t speed=Default::speed;
            std::chrono::milliseconds timeout=Default::timeout; 
            std::shared_ptr<abstract::Codec> codec = nullptr; 
        };

        /// Constructor with args struct.
        explicit Software(Args args);

        /// Constructor with specified arguments.
        Software(
            std::string port, 
            speed_t speed, 
            std::chrono::milliseconds timeout=Default::timeout, 
            std::shared_ptr<abstract::Codec> codec = nullptr
        ) : Software(Args{port, speed, timeout, codec}) {}

        /// Default constructor using default configuration.
        Software() : Software(Default::port, Default::speed) {}

        /// Destructor for proper cleanup.
        virtual ~Software() {}

        std::string post(std::string_view method_name, std::string_view json_string) override;

        void reconnect() override { ser_->reconnect(); }
        void disconnect() override { ser_->disconnect();  }
        bool isConnected() const override { return ser_->isConnected(); }

        int send(byte_view buffer) override { return ser_->sendCodec(codec_, buffer); }
        byte_view receive() override { return ser_->receiveCodec(codec_, [] (byte_view) { return true; } ); }
        byte_view receive(std::function<bool(byte_view)> filter) override { return ser_->receiveCodec(codec_, std::move(filter)); }
    };
}

#else
typedef void* nexus_serial_software_t;

nexus_serial_software_t nexus_serial_software_new(nexus_serial_hardware_t ser, nexus_codec_t codec);

nexus_serial_hardware_callback_id_t nexus_serial_software_add_callback(nexus_serial_software_t ser, void (*callback)(const uint8_t* buffer, size_t length));
void nexus_serial_software_remove_callback(nexus_serial_software_t ser, nexus_serial_hardware_callback_id_t callback);

#endif
#endif
