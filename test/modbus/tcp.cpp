#include "gtest/gtest.h"
#include "nexus/modbus/tcp/server.h"
#include "nexus/modbus/tcp/client.h"
#include "nexus/tools/await.h"
#include <thread>
#include <etl/keywords.h>

using namespace std::literals;

fun static convertToHexString(const std::vector<uint8_t>& data) {
    std::stringstream ss;
    for (const auto& byte : data) {
        // setw(2) ensures that each byte is represented with at least 2 characters
        // setfill('0') pads with leading zeros if necessary
        ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(byte);
    }
    // Reset stream flags and convert the stringstream to a string
    return ss.str();
}

TEST(Modbus, tcp) {
    val host = std::string("127.0.0.1");
    val port = 5001;

    var server = nexus::modbus::tcp::Server();
    var coils = std::vector<bool>({false, false});
    server.CoilGetter(0x0010, [&coils] { return coils[0];});
    server.CoilGetter(0x0011, [&coils] { return coils[1];});
    server.CoilSetter(0x0010, [&coils] (bool value) { coils[0] = value;});
    server.CoilSetter(0x0011, [&coils] (bool value) { coils[1] = value;});

    var analog_inputs = std::vector<uint16_t>({0xabcd, 0xef12});
    server.AnalogInputGetter(0x1010, [&analog_inputs] { return analog_inputs[0];});
    server.AnalogInputGetter(0x1011, [&analog_inputs] { return analog_inputs[1];});

    server.setLogger([] (const char* ip, nexus::byte_view request, nexus::byte_view response) {
        std::cout << "IP:" << ip << ", Req:" << convertToHexString(request) << ", Res:" << convertToHexString(response) << std::endl;
    });

    var future = std::async(std::launch::async, [&server, host, port] { return server.listen(host, port); });
    std::cout << "Server is running on http://" << host << ":" << port << std::endl;
    std::this_thread::sleep_for(1s);

    var client = nexus::modbus::tcp::Client(host, port);
    client.WriteMultipleCoils(0x0010, 2, {{false}, {true}});
    EXPECT_EQ(coils[0], false);
    EXPECT_EQ(coils[1], true);

    var [res, err] = client.ReadCoils(0x0010, 2);
    EXPECT_EQ(res[0], false);
    EXPECT_EQ(res[1], true);

    var [res2, err2] = client.ReadInputRegisters(0x1010, 2);
    EXPECT_EQ(res2[0], 0xabcd);
    EXPECT_EQ(res2[1], 0xef12);

    server.stop();
    var r = nexus::await | future;
    std::cout << (r == nexus::tcp::Server::Error::NONE ? "Server stop" : "Server fail to start in the first place") << int(r) << std::endl;
}