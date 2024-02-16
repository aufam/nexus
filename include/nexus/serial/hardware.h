#ifndef PROJECT_NEXUS_SERIAL_LL_H
#define PROJECT_NEXUS_SERIAL_LL_H

#include "nexus/abstract/serial.h"
#include "nexus/abstract/codec.h"
#include <termios.h>

#ifdef __cplusplus
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <list>
#include <unordered_map>
#include <memory>

namespace Project::nexus::serial {

    class Hardware : public abstract::Serial {
    public:
        using Callback = std::function<void(byte_view)>;
        using CallbackList = std::list<std::function<void(byte_view)>>;
        using CallbackId = CallbackList::iterator;

        using CodecList = std::unordered_map<std::shared_ptr<abstract::Codec>, CallbackList>;
        using CodecId = abstract::Codec*;

        using ReceiveFilter = std::function<bool(byte_view)>;

        Hardware(std::string port, speed_t speed, std::chrono::milliseconds timeout, std::shared_ptr<abstract::Codec> codec);
        virtual ~Hardware();

        void reconnect() override;
        void disconnect() override;
        bool isConnected() const override;

        int send(byte_view buffer) override;
        byte_view receive() override;
        byte_view receive(std::function<bool(byte_view)> filter) override;
        
        int sendCodec(std::shared_ptr<abstract::Codec> codec, byte_view buffer);
        byte_view receiveCodec(std::shared_ptr<abstract::Codec> codec, std::function<bool(byte_view)> filter);

        CodecId addCodec(std::shared_ptr<abstract::Codec> codec);
        void removeCodec(CodecId codec);

        CallbackId addCallback(Callback callback);
        void removeCallback(CallbackId callback);

        std::string port; 
        speed_t speed;
        std::chrono::milliseconds timeout; 

    protected:
        
        // file descriptor
        int fd = -1;

        // tx and rx buffer
        uint8_t rxBuffer[256] = {};
        std::mutex txMutex;
        std::mutex rxMutex;

        // received message
        struct MessageHandler {
            explicit MessageHandler(std::shared_ptr<abstract::Codec> codec) : codec(codec) {}
            std::shared_ptr<abstract::Codec> codec;
            std::vector<uint8_t> message;
            CallbackList callbacks;
            std::mutex mtx;
            std::condition_variable cv;
        };

        template <typename T>
        auto makeMessageHandler() { return std::make_unique<MessageHandler>(std::make_shared<T>()); }

        auto makeMessageHandler(std::shared_ptr<abstract::Codec> codec) { return std::make_unique<MessageHandler>(codec); }

        std::list<std::unique_ptr<MessageHandler>> messageHandlers;
        std::mutex codecMutex;

    private:
        // worker thread
        std::atomic<bool> isRunning = true;
        std::thread worker;
        void work();

    public:
        /// Default Codec for Serial Hardware.
        class Codec : public abstract::Codec {
        public:
            Codec() = default;
            virtual ~Codec() {}

            std::string path() const override { return "/default_serial_hardware_codec"; }
            byte_view encode(byte_view buffer) const override;
            byte_view decode(byte_view buffer) const override;
        };
    
        friend class Interface;
        class Interface {
        public:
            explicit Interface(std::shared_ptr<Hardware> ser);
            Interface(std::shared_ptr<Hardware> ser, std::shared_ptr<abstract::Codec> codec);
            ~Interface();

            CallbackId addCallback(Callback callback);
            void removeCallback(CallbackId callback);

            std::shared_ptr<Hardware> getSerialHardware() const { return ser_; }
            std::shared_ptr<abstract::Codec> getCodec() const { return codec_; }

        protected:
            Interface() = default; // for late init

            std::shared_ptr<Hardware> ser_;
            std::shared_ptr<abstract::Codec> codec_;
            std::list<CallbackId> callbacks_;
        };
    };
}

#else
typedef void* nexus_serial_hardware_t;
typedef void* nexus_serial_hardware_codec_id_t;
typedef void* nexus_serial_hardware_callback_id_t;

nexus_serial_hardware_t nexus_serial_hardware_new(const char* port, speed_t speed, int timeout, nexus_codec_t codec);

int nexus_serial_hardware_send_codec(nexus_serial_hardware_t ser, nexus_codec_t codec, const uint8_t* buffer, size_t length);
uint8_t* nexus_serial_hardware_receive_codec(nexus_serial_hardware_t ser, nexus_codec_t codec, int (*filter)(const uint8_t* buffer, size_t length), size_t* length);

nexus_serial_hardware_codec_id_t nexus_serial_hardware_add_codec(nexus_serial_hardware_t ser, nexus_codec_t codec);
void nexus_serial_hardware_remove_codec(nexus_serial_hardware_t ser, nexus_serial_hardware_codec_id_t codec);

nexus_serial_hardware_callback_id_t nexus_serial_hardware_add_callback(nexus_serial_hardware_t ser, void (*callback)(const uint8_t* buffer, size_t length));
void nexus_serial_hardware_remove_callback(nexus_serial_hardware_t ser, nexus_serial_hardware_callback_id_t callback);

#endif
#endif
