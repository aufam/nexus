#ifndef PROJECT_NEXUS_TCP_CLIENT_H
#define PROJECT_NEXUS_TCP_CLIENT_H

#ifdef __cplusplus
#include <functional>
#include <atomic>
#include <chrono>
#include "nexus/tools/byte_view.h"

namespace Project::nexus::tcp {

    class Client {
    public:
        struct Args {
            std::string host; 
            int port;
        };
        
        Client(Args args);
        Client(std::string host, int port) : Client({.host=host, .port=port}) {}
        virtual ~Client() {}

        enum Error {
            NONE,
            SOCKET,
            CONNECT,
            SELECT,
            GETSOCKOPT,
            SEND,
            RECV,
        };

        std::pair<std::vector<uint8_t>, Error> request(
            nexus::byte_view buffer,
            std::chrono::milliseconds timeout=std::chrono::milliseconds(1000)
        );

    protected:
        Args _args;
    };
}

#else
#include "nexus/tcp/c_wrapper.h"
#endif
#endif