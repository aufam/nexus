#include "gtest/gtest.h"
#include "nexus/serial/serial.h"
#include "nexus/tools/await.h"
#include <etl/keywords.h>

using namespace std::literals;

class Comm : virtual public nexus::serial::Serial {
public:
    Comm() : nexus::serial::Serial() {
        std::cout << "Comm start\n";
        worker = std::thread(&Comm::work, this);
    }

    virtual ~Comm() {
        isRunning = false;
        worker.join();
        std::cout << "Comm end\n";
    }

    void reconnect(Args) override {}
    void disconnect() override { }
    bool isConnected() const override { return true; }

private:
    void work() {
        std::vector<std::string> random ({
            "123", 
            "456",
        });
        
        while (isRunning) {
            var now = std::chrono::steady_clock::now();
            for (val &s in random) {
                {
                    std::unique_lock<std::mutex> lock(receivedMessageMutex);
                    receivedMessageCondition.wait_until(lock, now + 100ms, [this] { return not isRunning; });
                    now = std::chrono::steady_clock::now();
                }

                {
                    std::lock_guard<std::mutex> lock(receivedMessageMutex);
                    receivedMessage = std::vector<uint8_t>(s.begin(), s.end());
                }
                std::cout << "produce: " << s << std::endl;
                receivedMessageCondition.notify_one();
                for (var &callback in callbacks) {
                    callback({receivedMessage.data(), receivedMessage.size()});
                }
            }
        }
    }

    std::atomic<bool> isRunning = true;
    std::thread worker;
};

TEST(Serial, serial) {
    var comm = std::make_unique<Comm>();

    var sa1 = std::async(std::launch::async, [&comm] { return comm->receiveText(); });
    var sa2 = std::async(std::launch::async, [&comm] { return comm->receiveText(); });

    var s1 = nexus::await | sa1;
    var s2 = nexus::await | sa2;

    // var s1 = comm->receiveText();
    // var s2 = comm->receiveText();

    std::cout << "s1: " << s1 << std::endl;
    std::cout << "s2: " << s2 << std::endl;

    EXPECT_EQ(s1, "123");
    EXPECT_EQ(s2, "456");

    val buf = comm->receiveText([] (std::string_view text) { return text == "456"; });
    val s3 = std::string(buf.begin(), buf.end());
    std::cout << "s3: " << s3 << std::endl;
    EXPECT_EQ(s3, "456");
}