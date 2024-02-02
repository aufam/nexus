#include "nexus/tcp/server.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <future>
#include <thread>
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

static int set_non_blocking(int socket) {
    return ::fcntl(socket, F_SETFL, ::fcntl(socket, F_GETFL, 0) | O_NONBLOCK);
} 

static bool is_socket_alive(int client_socket) {
    uint8_t dummy;
    auto res = ::recv(client_socket, &dummy, 1, MSG_PEEK);
    if (res == 0)
        return false; // connection close
    if (res > 0)
        return true; // data available, socket is alive
    if (res < 0 && (errno == EWOULDBLOCK || errno == EAGAIN))
        return true; // no data available, socket is alive
    return false;
}

static std::vector<uint8_t> receive_buffer(int client_socket) {
    std::vector<uint8_t> res(MAX_HANDLE_SZ);
    auto data = res.data();
    bool retried = false;

    while (true) {
        auto current_size = res.size();
        auto size = ::recv(client_socket, data, MAX_HANDLE_SZ, 0);

        if (size <= 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                if (not retried) {
                    // the incoming data might be exceed MAX_HANDLE_SZ 
                    retried = true;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                    continue;
                } else {
                    // the incoming data is exactly MAX_HANDLE_SZ 
                    res.resize(current_size - MAX_HANDLE_SZ);
                    break;
                }
            } else {
                // client socket is closed
                return {};
            }
        }

        retried = false;
        
        if (size == MAX_HANDLE_SZ) {
            res.resize(current_size + size);
            data = res.data() + current_size;
        } else {
            res.resize(current_size + size - MAX_HANDLE_SZ);
            break;
        }
    }

    return res;
}

tcp::Server::~Server() {
    stop();
}

fun tcp::Server::listen(std::string host, int port) -> Error {
    server_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        return Error::SOCKET;

    auto se = etl::ScopeExit([this] { stop(); });

    // Bind the socket to an IP address and port
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = ::htons(port);

    // Convert host string to IP address
    if (inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr) <= 0)
        return Error::PTON;

    int enable = 1;
    if (::setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        return Error::OPT;

    if (::bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
        return Error::BIND;

    // Listen for incoming connections
    constexpr int MAX_SOCKET = 5;
    if (::listen(server_socket, MAX_SOCKET) < 0)
        return Error::LISTEN;

    set_non_blocking(server_socket);

    is_running = true;
    std::vector<std::thread> threads;

    while (is_running) {
        int client_socket = ::accept(server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // No incoming connection, continue or sleep briefly
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            // Handle other errors
            continue;
        }

        set_non_blocking(client_socket);

        std::ignore = std::async(std::launch::async, [this, client_socket] {
            auto se = etl::ScopeExit([client_socket] {
                // Optionally check if the socket is still connected before closing
                if (::shutdown(client_socket, SHUT_RDWR) == 0) {
                    ::close(client_socket);
                }
            });

            // Get client's IP address
            sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            char client_ip[INET_ADDRSTRLEN] = {};
            if (::getpeername(client_socket, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len) == 0) {
                ::inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            }

            while (is_running && is_socket_alive(client_socket)) {
                auto req = receive_buffer(client_socket);
                if (req.empty()) 
                    continue;
                
                auto res = std::vector<uint8_t>();
                for (auto &callback : callbacks) {
                    res = callback(req);
                    if (not res.empty()) break;
                }

                if (not res.empty()) ::send(client_socket, res.data(), res.size(), 0);
                if (logger) logger(client_ip, req, res);
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return Error::NONE;
}

fun tcp::Server::stop() -> void {
    is_running = false;
    if (server_socket >= 0) {
        ::close(server_socket);
        server_socket = -1;
    }
}

fun tcp::Server::addCallback(std::function<std::vector<uint8_t>(nexus::byte_view)> callback) -> Server& {
    callbacks.push_back(std::move(callback));
    return *this;
}

fun tcp::Server::setLogger(std::function<void(const char*, nexus::byte_view, nexus::byte_view)> logger) -> Server& {
    this->logger = std::move(logger);
    return *this;
}

extern "C" {
    typedef void* nexus_tcp_server_t;

    nexus_tcp_server_t nexus_tcp_server_new() {
        return new tcp::Server();
    }

    void nexus_tcp_server_delete(nexus_tcp_server_t server) {
        delete static_cast<tcp::Server*>(server);
    }

    int nexus_tcp_server_listen(nexus_tcp_server_t server, const char* host, int port) {
        return static_cast<tcp::Server*>(server)->listen(host, port);
    }

    void nexus_tcp_server_stop(nexus_tcp_server_t server) {
        static_cast<tcp::Server*>(server)->stop();
    }

    int nexus_tcp_server_is_running(nexus_tcp_server_t server) {
        return static_cast<tcp::Server*>(server)->isRunning();
    }

    void nexus_tcp_server_add_callback(nexus_tcp_server_t server, uint8_t* (*callback)(const uint8_t* buffer, size_t* length)) {
        static_cast<tcp::Server*>(server)->addCallback([callback] (byte_view buffer) -> std::vector<uint8_t> {
            var length = buffer.len();
            var res = callback(buffer.data(), &length);
            var se = etl::ScopeExit([res] { ::free(res); });
            return {res, res + length};
        });
    }

    void nexus_tcp_server_set_logger(
        nexus_tcp_server_t server, 
        void (*logger)(const char* ip, const uint8_t* req_buf, size_t req_len, const uint8_t* res_buf, size_t res_len)
    ) {
        static_cast<tcp::Server*>(server)->setLogger([logger] (const char* ip, byte_view req, byte_view res) {
            logger(ip, req.data(), req.len(), res.data(), res.len());
        });
    }
}