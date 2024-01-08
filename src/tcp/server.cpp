#include "nexus/tcp/server.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <future>
#include <thread>
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

tcp::Server::~Server() {
    stop();
}

fun tcp::Server::listen(std::string host, int port) -> Error {
    server_socket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        return Error::SOCKET;
    }

    // Bind the socket to an IP address and port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = ::htons(port);

    // Convert host string to IP address
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0) {
        ::close(server_socket);
        return Error::PTON;
    }

    int enable = 1;
    if (::setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        ::close(server_socket);
        return Error::OPT;
    }

    if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        ::close(server_socket);
        return Error::BIND;
    }

    // Listen for incoming connections
    if (::listen(server_socket, 5) < 0) {
        ::close(server_socket);
        return Error::LISTEN;
    }

    int flags = ::fcntl(server_socket, F_GETFL, 0);
    ::fcntl(server_socket, F_SETFL, flags | O_NONBLOCK);

    is_running = true;

    while (is_running) {
        int client_socket = ::accept(server_socket, nullptr, nullptr);

        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No incoming connection, continue or sleep briefly
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            // Handle other errors
        }

        std::ignore = std::async(std::launch::async, [this, client_socket] {
            auto se = etl::ScopeExit([client_socket] { ::close(client_socket); });

            // Get client's IP address
            sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            char client_ip[INET_ADDRSTRLEN] = {};
            if (::getpeername(client_socket, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len) == 0) {
                ::inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            }
            
            auto buffer = std::make_unique<uint8_t[]>(1024);
            auto n = ::recv(client_socket, buffer.get(), 1024, 0);
            if (n < 0) return;

            auto req = nexus::byte_view{buffer.get(), static_cast<size_t>(n)};
            std::vector<uint8_t> res;
            for (auto &callback : callbacks) {
                res = callback(req);
                if (not res.empty()) break;
            }

            if (logger) logger(client_ip, req, res);
            if (not res.empty()) ::send(client_socket, res.data(), res.size(), 0);
        });   
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return Error::NONE;
}

fun tcp::Server::stop() -> void {
    is_running = false;
    if (server_socket >= 0)
        ::close(server_socket);
}

fun tcp::Server::addCallback(std::function<std::vector<uint8_t>(nexus::byte_view)> callback) -> Server& {
    callbacks.push_back(std::move(callback));
    return *this;
}

fun tcp::Server::setLogger(std::function<void(const char*, nexus::byte_view, nexus::byte_view)> logger) -> Server& {
    this->logger = std::move(logger);
    return *this;
}