#include "nexus/serial/hardware.h"
#include "nexus/tools/detect_virtual_comm.h"
#include "nexus/tools/json.h"
#include <future>
#include <algorithm>
#include <fcntl.h>   /* File Control Definitions */
#include <unistd.h>  /* UNIX Standard Definitions */
#include <etl/keywords.h>

using namespace nexus;
using namespace std::literals;

fun serial::Hardware::Codec::encode(byte_view buffer) const -> byte_view {
    return buffer;
}

fun serial::Hardware::Codec::decode(byte_view buffer) const -> byte_view {
    return buffer.len() == 0 or buffer.back() != '\n' ? null : buffer;
}

serial::Hardware::Interface::Interface(std::shared_ptr<Hardware> ser) : ser_(ser), codec_(ser->messageHandlers.begin()->get()->codec) {}

serial::Hardware::Interface::Interface(std::shared_ptr<Hardware> ser, std::shared_ptr<abstract::Codec> codec) 
    : ser_(ser), codec_(codec) 
{
    ser_->addCodec(codec);
}

serial::Hardware::Interface::~Interface() {
    std::lock_guard<std::mutex> lockCodec(ser_->codecMutex);
    var handler = std::find_if(ser_->messageHandlers.begin(), ser_->messageHandlers.end(), [this](val &item) {
        return item->codec == this->codec_;
    });

    if (handler == ser_->messageHandlers.end())
        return;
    
    for (var callback in callbacks_)
        handler->get()->callbacks.erase(callback);
}

fun serial::Hardware::Interface::addCallback(std::function<void(byte_view)> callback) -> CallbackId {
    std::lock_guard<std::mutex> lockCodec(ser_->codecMutex);
    var handler = std::find_if(ser_->messageHandlers.begin(), ser_->messageHandlers.end(), [this](val &item) {
        return item->codec == this->codec_;
    });

    handler->get()->callbacks.push_back(std::move(callback));
    var it = std::prev(handler->get()->callbacks.end());
    callbacks_.push_back(it);
    return it;
}

fun serial::Hardware::Interface::removeCallback(CallbackId callback) -> void {
    std::lock_guard<std::mutex> lockCodec(ser_->codecMutex);
    var handler = std::find_if(ser_->messageHandlers.begin(), ser_->messageHandlers.end(), [this](val &item) {
        return item->codec == this->codec_;
    });

    handler->get()->callbacks.erase(callback);
    callbacks_.remove(callback);
}

serial::Hardware::Hardware(std::string port, speed_t speed, std::chrono::milliseconds timeout, std::shared_ptr<abstract::Codec> codec)
    : port(std::move(port))
    , speed(speed)
    , timeout(timeout)
{
    if (not codec)
        codec = std::make_shared<Hardware::Codec>();

    messageHandlers.push_back(makeMessageHandler(codec));
    reconnect();
    worker = std::thread(&Hardware::work, this);
}

serial::Hardware::~Hardware() {
    disconnect();
    isRunning = false;
    worker.join();
}

fun serial::Hardware::reconnect() -> void {
    std::scoped_lock lockTx(txMutex);
    std::scoped_lock lockRx(rxMutex);

    if (isConnected())
        disconnect();

    var ports = std::vector<std::string>();
    if (this->port == null or this->port == std::string("auto")) {
        ports = tools::detectVirtualComm();
        if (not ports.empty())
            this->port = ports[0];
    }

    fd = open(this->port.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
        return;

    struct termios tty = {};
    if (tcgetattr(fd, &tty) != 0) {
        disconnect();
        return;
    }

    cfsetospeed(&tty, this->speed);
    cfsetispeed(&tty, this->speed);

    tty.c_cflag |= (CLOCAL | CREAD);    /* ignore modem controls */
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 /* 8-bit characters */
    tty.c_cflag &= ~(PARENB | PARODD);  /* no parity bit */
    tty.c_cflag &= ~CSTOPB;             /* only need 1 stop bit */
    tty.c_cflag &= ~CRTSCTS;            /* no hardware flow control */

    // /* setup for non-canonical mode */
    tty.c_iflag &= ~(IGNBRK | IXOFF | IXON | IXANY | INLCR | ICRNL);
    tty.c_lflag = 0; // no signaling chars, no echo, no canonical processing
    tty.c_oflag = ~OPOST; // no remapping

    int vtime = this->timeout.count() / 100;
    vtime = etl::clamp(vtime, 0, 10);

    tty.c_cc[VTIME] = (cc_t) (vtime); 
    tty.c_cc[VMIN]  = 0; 

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        disconnect();
        return;
    }

    // clear rx buffer
    tcflush(fd, TCIOFLUSH);
}

fun serial::Hardware::disconnect() -> void {
    if (isConnected()) {
        close(fd);
        fd = -1;
    }
}

fun serial::Hardware::isConnected() const -> bool {
    return fd >= 0;
}

fun serial::Hardware::send(byte_view buffer) -> int {
    return sendCodec(messageHandlers.begin()->get()->codec, buffer);
}

fun serial::Hardware::receive(std::function<bool(byte_view)> filter) -> byte_view {
    return receiveCodec(messageHandlers.begin()->get()->codec, std::move(filter));
}

fun serial::Hardware::sendCodec(std::shared_ptr<abstract::Codec> codec, byte_view buffer) -> int {
    if (not isConnected()) 
        return -1;

    std::scoped_lock lock(txMutex);
    val buf = codec->encode(buffer);
    val res = write(fd, buf.data(), buf.size());
    if (res < 0)
        disconnect();

    return (int) res;
}

fun serial::Hardware::receiveCodec(std::shared_ptr<abstract::Codec> codec, std::function<bool(byte_view)> filter) -> byte_view {
    if (not isConnected()) 
        return null;
    
    var handler = std::find_if(messageHandlers.begin(), messageHandlers.end(), [&codec](val &item) {
        return item->codec == codec;
    });

    if (handler == messageHandlers.end())
        return null;
    
    std::unique_lock<std::mutex> lock(handler->get()->mtx);
    return handler->get()->cv.wait_for(lock, timeout, [this, handler, fn=std::move(filter)] {
        return not isRunning or (not handler->get()->message.empty() and fn(handler->get()->message));
    }) and isRunning ? std::move(handler->get()->message) : null;
}

fun serial::Hardware::addCodec(std::shared_ptr<abstract::Codec> codec) -> CodecId {
    std::lock_guard<std::mutex> lock(codecMutex);
    var handler = std::find_if(messageHandlers.begin(), messageHandlers.end(), [&codec](val &item) {
        return item->codec == codec;
    });

    if (handler == messageHandlers.end())
        messageHandlers.push_back(makeMessageHandler(codec));
    
    return codec.get();
}

fun serial::Hardware::removeCodec(CodecId codec) -> void {
    std::lock_guard<std::mutex> lock(codecMutex);
    auto it = std::find_if(messageHandlers.begin(), messageHandlers.end(), [codec](val &item) { 
        return item->codec.get() == codec; 
    });

    if (it != messageHandlers.end() and it != messageHandlers.begin())
        messageHandlers.erase(it);
}

fun serial::Hardware::addCallback(std::function<void(byte_view)> callback) -> CallbackId {
    std::lock_guard<std::mutex> lock(codecMutex);
    var &callbackList = messageHandlers.begin()->get()->callbacks;
    callbackList.push_back(std::move(callback));
    return std::prev(callbackList.end());
}

void serial::Hardware::removeCallback(CallbackId callback) {
    std::lock_guard<std::mutex> lock(codecMutex);
    messageHandlers.begin()->get()->callbacks.erase(callback);
}

fun serial::Hardware::work() -> void {
    size_t rxIndex = 0;
    val launch_callback = [this] (const CallbackList& callbackList, byte_view receivedMessage) {
        if (callbackList.empty())
            return;
        
        std::ignore = std::async(std::launch::async, [this, &callbackList, receivedMessage] {
            for (var &callback in callbackList)
                callback(receivedMessage);
        });
    };
            
    val try_decode = [&, this] {
        for (val i in etl::range(rxIndex + 1)) {
            val len = rxIndex + 1 - i;
            var processed = false;

            codecMutex.lock();
            for (var &handler in messageHandlers) {
                var receivedMessage = handler->codec->decode(byte_view{rxBuffer + i, len});
                if (receivedMessage.empty())
                    continue;
                
                processed = true;
                launch_callback(handler->callbacks, receivedMessage.copy());
                
                std::lock_guard<std::mutex> lock(handler->mtx);
                handler->message = receivedMessage.copy();
                handler->cv.notify_one();
            }
            codecMutex.unlock();
            
            if (not processed) 
                continue;
            
            rxIndex = 0;
            return;
        }

        rxIndex++;
        rxIndex %= sizeof(rxBuffer);
    };

    val try_read = [&, this] {
        std::scoped_lock lock(rxMutex);
        return read(fd, rxBuffer + rxIndex, 1);
    };

    while (isRunning) {
        if (not isConnected()) {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        val n = try_read();

        // success
        if (n > 0) 
            try_decode();
        
        // timeout
        elif (n == 0)
            continue;

        // disconnected
        else 
            disconnect();
    }
}

extern "C" {
    typedef void* nexus_serial_hardware_t;
    typedef void* nexus_serial_hardware_callback_id_t;
    typedef void* nexus_codec_id_t;
    typedef void* nexus_codec_t;

    nexus_serial_hardware_t nexus_serial_hardware_new(const char* port, speed_t speed, int timeout, nexus_codec_t codec) {
        return new serial::Hardware(port, speed, std::chrono::milliseconds(timeout), 
            std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec)));
    }

    int nexus_serial_hardware_send_codec(nexus_serial_hardware_t ser, nexus_codec_t codec, const uint8_t* buffer, size_t length) {
        return static_cast<serial::Hardware*>(ser)->sendCodec(
            std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec)), 
            byte_view{buffer, length}
        );
    }

    uint8_t* nexus_serial_hardware_receive_codec(nexus_serial_hardware_t ser, nexus_codec_t codec, int (*filter)(const uint8_t* buffer, size_t length), size_t* length) {
        auto res = static_cast<serial::Hardware*>(ser)->receiveCodec(
            std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec)), 
            [filter] (byte_view buffer) { return filter(buffer.data(), buffer.len()); }
        );
        
        auto ret = (uint8_t*) ::malloc(res.len());
        ::memcpy(ret, res.data(), res.len());
        *length = res.len();
        return ret;
    }

    void nexus_serial_hardware_add_codec(nexus_serial_hardware_t ser, nexus_codec_t codec) {
        static_cast<serial::Hardware*>(ser)->addCodec(std::shared_ptr<abstract::Codec>(static_cast<abstract::Codec*>(codec)));
    }

    void nexus_serial_hardware_remove_codec(nexus_serial_hardware_t ser, nexus_codec_t codec) {
        static_cast<serial::Hardware*>(ser)->removeCodec(static_cast<abstract::Codec*>(codec));
    }

    nexus_serial_hardware_callback_id_t nexus_serial_hardware_add_callback(nexus_serial_hardware_t ser, void (*callback)(const uint8_t* buffer, size_t length)) {
        var it = static_cast<serial::Hardware*>(ser)->addCallback([callback] (byte_view buffer) { callback(buffer.data(), buffer.len()); });
        var res = ::malloc(sizeof(it));
        ::memcpy(res, &it, sizeof(it));
        return res;
    }

    void nexus_serial_hardware_remove_callback(nexus_serial_hardware_t ser, nexus_serial_hardware_callback_id_t callback) {
        static_cast<serial::Hardware*>(ser)->removeCallback(*reinterpret_cast<serial::Hardware::CallbackId*>(callback));
        ::free(callback);
    }
}