#include "nexus/tcp/client.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <future>
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

tcp::Client::Client(std::string host, int port, std::chrono::milliseconds timeout) : args{host, port, timeout} {
    reconnect();
}

tcp::Client::~Client() {
    disconnect();
}

fun tcp::Client::error() const -> Error {
    return error_;
}

fun tcp::Client::disconnect() -> void {
    if (client_socket >= 0) {
        ::close(client_socket);
        client_socket = -1;
    }
}

fun tcp::Client::reconnect() -> void {
    val set_error = [this] (Error err) { error_ = err; setup_error = err; disconnect(); };
    
    // Create socket
    client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    
    if (client_socket < 0)
        return set_error(Error::SOCKET);

    // Set socket to non-blocking mode
    ::fcntl(client_socket, F_SETFL, O_NONBLOCK);
    
    // Connect to the server
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = ::htons(args.port);
    server_addr.sin_addr.s_addr = ::inet_addr(args.host.c_str()); // Server IP address

    if (::connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        // Handle connection error
        if (errno != EINPROGRESS)
            return set_error(Error::CONNECT);

        // Wait for the connection to be established with a timeout
        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(client_socket, &write_fds);

        struct timeval tv;
        tv.tv_sec = args.timeout.count() / 1000;
        tv.tv_usec = (args.timeout.count() % 1000) * 1000;

        if (::select(client_socket + 1, nullptr, &write_fds, nullptr, &tv) <= 0)
            return set_error(Error::CONNECT_TIMEOUT);

        // Connection established or error occurred
        int error = 0;
        socklen_t len = sizeof(error);

        if (::getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &error, &len) != 0 || error != 0)
            return set_error(Error::GETSOCKOPT);
    }

    setup_error = Error::NONE;
    error_ = Error::NONE;
}

fun tcp::Client::isConnected() const -> bool {
    return client_socket >= 0;
}

fun tcp::Client::request(nexus::byte_view buffer) -> nexus::byte_view {
    if (setup_error != Error::NONE) 
        return set_error_(nexus::byte_view(), setup_error);

    var n = ::send(client_socket, buffer.data(), buffer.size(), 0);
    if (n <= 0)
        return set_error_(nexus::byte_view(), Error::SEND);

    if (not receiveTimeout(args.timeout))
        return {};

    std::vector<uint8_t> res(MAX_HANDLE_SZ);
    n = ::recv(client_socket, res.data(), MAX_HANDLE_SZ, 0);
    if (n <= 0)
        return set_error_(nexus::byte_view(), Error::RECV);

    res.resize(n);
    return set_error_(std::move(res), Error::NONE);
}

fun tcp::Client::receiveTimeout(std::chrono::milliseconds timeout) -> bool {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(client_socket, &readfds);

    struct timeval tv;
    tv.tv_sec = timeout.count() / 1000;
    tv.tv_usec = (timeout.count() % 1000) * 1000;

    auto res = select(client_socket + 1, &readfds, nullptr, nullptr, &tv) > 0;
    return set_error_(res, res ? Error::RECV_TIMEOUT : Error::NONE);
}

extern "C" {
    typedef void* nexus_tcp_client_t;

    nexus_tcp_client_t nexus_tcp_client_new(const char* host, int port, int timeout) {
        return new tcp::Client(host, port, std::chrono::milliseconds(timeout));
    }
}