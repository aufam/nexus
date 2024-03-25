#include "nexus/modbus/rtu/server.h"
#include <etl/keywords.h>

using namespace nexus;

modbus::rtu::Server::Server(int server_address) : api::Server(server_address) {}

bool modbus::rtu::Server::listen(std::string port, speed_t speed, std::chrono::milliseconds timeout) {
    if (is_running) 
        return false;
    
    auto ser = serial::Hardware(port, speed, timeout, std::make_shared<api::Codec>());
    ser.addCallback([this, &ser] (byte_view buffer) {
        auto req = buffer.copy();
        var res = process_callback(buffer);
        if (res.empty())
            return;

        if (logger_)
            logger_(req, res);
        
        ser.send(res);
    });

    is_running = true;
    while (is_running and ser.isConnected()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, timeout, [this] { return not is_running; });
    }

    auto res = not is_running;
    is_running = false;
    return res;
}

bool modbus::rtu::Server::listen(std::shared_ptr<serial::Hardware> ser) {
    if (is_running) 
        return false;
    
    auto iface = serial::Hardware::Interface(ser, std::make_shared<api::Codec>());
    iface.addCallback([this, &iface] (byte_view buffer) {
        auto req = buffer.copy();
        var res = process_callback(buffer);
        if (res.empty())
            return;

        if (logger_)
            logger_(req, res);
        
        iface.getSerialHardware()->sendCodec(iface.getCodec(), res);
    });

    is_running = true;
    while (is_running and iface.getSerialHardware()->isConnected()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, ser->timeout, [this] { return not is_running; });
    }

    auto res = not is_running;
    is_running = false;
    return res;
}

void modbus::rtu::Server::stop() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        is_running = false;
    }
    cv.notify_one();
}

fun modbus::rtu::Server::setLogger(std::function<void(byte_view, byte_view)> logger) -> Server& {
    logger_ = std::move(logger);
    return *this;
}

modbus::rtu::Server::~Server() {
    stop();
}

extern "C" {
    typedef void* nexus_modbus_rtu_server_t;
    typedef void* nexus_serial_ll_t;

    nexus_modbus_rtu_server_t nexus_modbus_rtu_server_new(int server_address) {
        return new modbus::rtu::Server(server_address);
    }

    void nexus_modbus_rtu_server_set_logger(nexus_modbus_rtu_server_t server, void (*logger)(const uint8_t* req_buf, size_t req_len, const uint8_t* res_buf, size_t res_len)) {
        static_cast<modbus::rtu::Server*>(server)->setLogger([logger] (byte_view req, byte_view res) {
            logger(req.data(), req.len(), res.data(), res.len());
        });
    }

    void nexus_modbus_rtu_server_listen(nexus_modbus_rtu_server_t server, nexus_serial_ll_t ser) {
        static_cast<modbus::rtu::Server*>(server)->listen(std::shared_ptr<serial::Hardware>(static_cast<serial::Hardware*>(ser)));
    }

    void nexus_modbus_rtu_server_stop(nexus_modbus_rtu_server_t server) {
        static_cast<modbus::rtu::Server*>(server)->stop();
    }
}
