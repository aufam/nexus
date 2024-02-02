#ifndef PROJECT_NEXUS_TCP_CLIENT_H
#define PROJECT_NEXUS_TCP_CLIENT_H

#include "nexus/abstract/client.h"

#ifdef __cplusplus
#include <functional>
#include <atomic>
#include <chrono>

namespace Project::nexus::tcp {

    class Client : virtual public abstract::Client {
    public:
        struct Args {
            std::string host; 
            int port;
            std::chrono::milliseconds timeout = std::chrono::milliseconds(1000);
        };
        
        Client(std::string host, int port, std::chrono::milliseconds timeout = std::chrono::milliseconds(1000));
        explicit Client(Args args) : Client(args.host, args.port, args.timeout) {}
        virtual ~Client();

        void reconnect() override;
        void disconnect() override;
        bool isConnected() const override;

        nexus::byte_view request(nexus::byte_view buffer) override;

        enum Error {
            NONE,
            SOCKET,
            CONNECT,
            CONNECT_TIMEOUT,
            GETSOCKOPT,
            SEND,
            RECV,
            RECV_TIMEOUT,
        };
        Error error() const;

    protected:
        Args args;
        int client_socket = -1;
        Error setup_error = Error::NONE;
        Error error_ = Error::NONE;
    
    private:
        // helper methods
        bool receiveTimeout(std::chrono::milliseconds timeout);

        template <typename T, typename R = T>
        R set_error_(T&& t, Error err) {
            error_ = err;
            return std::forward<T>(t);
        }
    };
}

#else
typedef void* nexus_tcp_client_t;

nexus_tcp_client_t nexus_tcp_client_new(const char* host, int port, int timeout);
#endif
#endif