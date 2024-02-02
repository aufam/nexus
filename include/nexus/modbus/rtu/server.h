#ifndef PROJECT_NEXUS_MODBUS_RTU_SERVER_H
#define PROJECT_NEXUS_MODBUS_RTU_SERVER_H

#include "nexus/modbus/api_server.h"
#include "nexus/serial/hardware.h"

#ifdef __cplusplus

namespace Project::nexus::modbus::rtu {

    class Server : public api::Server {
    public:
        explicit Server(int server_address);
        virtual ~Server();

        std::string path() const override { return "/modbus_rtu_server"; };

        bool listen(std::string port, speed_t speed, std::chrono::milliseconds timeout = std::chrono::milliseconds(100));
        bool listen(std::shared_ptr<serial::Hardware> ser);

        void stop();
        bool isRunning() const override { return is_running; }

        Server& setLogger(std::function<void(nexus::byte_view, nexus::byte_view)> logger);

    protected:
        std::function<void(nexus::byte_view, nexus::byte_view)> logger_;

    private:
        std::mutex mtx;
        std::condition_variable cv;
        bool is_running = false;
    };
}

#else
typedef void* nexus_modbus_rtu_server_t;

nexus_modbus_rtu_server_t nexus_modbus_rtu_server_new(int server_address);

void nexus_modbus_rtu_server_set_logger(nexus_modbus_rtu_server_t server, void (*logger)(const uint8_t* req_buf, size_t req_len, const uint8_t* res_buf, size_t res_len));

void nexus_modbus_rtu_server_listen(nexus_modbus_rtu_server_t server, nexus_serial_ll_t ser);
void nexus_modbus_rtu_server_stop(nexus_modbus_rtu_server_t server);

#endif
#endif // PROJECT_NEXUS_MODBUS_RTU_SERVER_H