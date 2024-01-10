#include "gtest/gtest.h"
#include "nexus/tcp/server.h"
#include "nexus/tcp/client.h"
#include "nexus/tools/await.h"
#include <thread>
#include <etl/keywords.h>

using namespace std::literals;

TEST(TCP, server) {
    val host = std::string("127.0.0.1");
    val port = 5004;

    var server = nexus::tcp::Server(); server
    .addCallback([] (nexus::byte_view request) -> std::vector<uint8_t> {
        return request; // echo
    })
    .setLogger([] (const char* ip, nexus::byte_view request, nexus::byte_view response) {
        std::cout << "IP: " << ip << ", Req:" << std::string_view(request) << ", Res:" << std::string_view(response) << std::endl;
    });

    var res = std::async(std::launch::async, [&server, host, port] { return server.listen(host, port); });
    std::cout << "Server is running on http://" << host << ":" << port << std::endl;
    std::this_thread::sleep_for(1s);

    var client = nexus::tcp::Client(host, port);
    var [response, err] = client.request("Test");
    EXPECT_EQ(nexus::byte_view(response), nexus::byte_view("Test"));
    var [response2, err2] = client.request("Test 123");
    EXPECT_EQ(nexus::byte_view(response2), nexus::byte_view("Test 123"));

    server.stop();
    var r = nexus::await | res;
    std::cout << (r == nexus::tcp::Server::Error::NONE ? "Server stop" : "Server fail to start in the first place") << int(r) << std::endl;
}

extern "C" int c_tcp_server();

TEST(TCP, cserver) {
    EXPECT_EQ(c_tcp_server(), 0);
}