#include "nexus/tcp/client.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <future>
#include <etl/scope_exit.h>
#include <etl/keywords.h>

using namespace nexus;

tcp::Client::Client(Args args) : _args(args) {}

fun tcp::Client::request(nexus::byte_view buffer, std::chrono::milliseconds timeout) -> std::pair<std::vector<uint8_t>, Error> {
    // Create socket
    int client_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    
    if (client_socket < 0)
        return {{}, Error::SOCKET};

    var se = etl::ScopeExit([client_socket] { ::close(client_socket); });
    
    // Set socket to non-blocking mode
    ::fcntl(client_socket, F_SETFL, O_NONBLOCK);
    
    // Connect to the server
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = ::htons(_args.port);
    server_addr.sin_addr.s_addr = ::inet_addr(_args.host.c_str()); // Server IP address

    if (::connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0) {
        // Handle connection error
        if (errno != EINPROGRESS)
            return {{}, Error::CONNECT};

        // Wait for the connection to be established with a timeout
        fd_set write_fds;
        FD_ZERO(&write_fds);
        FD_SET(client_socket, &write_fds);

        struct timeval tv;
        val sec = timeout.count() / 1000;
        val usec = (timeout.count() % 1000) * 1000;
        tv.tv_sec = sec;
        tv.tv_usec = usec;

        if (::select(client_socket + 1, nullptr, &write_fds, nullptr, &tv) <= 0)
            return {{}, Error::SELECT};

        // Connection established or error occurred
        int error = 0;
        socklen_t len = sizeof(error);

        if (::getsockopt(client_socket, SOL_SOCKET, SO_ERROR, &error, &len) != 0 || error != 0)
            return {{}, Error::GETSOCKOPT};
    }

    // Set socket back to blocking mode
    ::fcntl(client_socket, F_SETFL, 0);

    if (::send(client_socket, buffer.data(), buffer.size(), 0) < 0)
        return {{}, Error::SEND};

    // Receive response from the server
    var res = std::make_unique<uint8_t[]>(1024);
    val n = ::recv(client_socket, res.get(), 1024, 0);
    if (n < 0) return {{}, Error::RECV};

    return {{res.get(), res.get() + n}, NONE};
}
