#include "gtest/gtest.h"
#include <chrono>
#include <thread>
#include "nexus/abstract/serial.h"
#include "nexus/tools/await.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

using namespace std::literals;

class Comm : virtual public nexus::abstract::Serial {
public:
    Comm() {
        std::cout << "Serial start\n";
        worker = std::thread(&Comm::work, this);
    }

    virtual ~Comm() {
        isRunning = false;
        worker.join();
        std::cout << "Serial end\n";
    }

    nexus::byte_view receive() override {  
        return receive([] (var) { return true; });
    };

    nexus::byte_view receive(std::function<bool(nexus::byte_view)> filter) override {
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this, &filter] { return not isRunning or (not receivedMessage.empty() and filter(receivedMessage)); });
        }
        auto res = std::move(receivedMessage);
        std::this_thread::sleep_for(1ms);
        return res;
    };
    

private:
    void work() {
        for (int i = 0; isRunning; ++i) {
            var now = std::chrono::steady_clock::now();
            {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait_until(lock, now + 100ms, [this] { return not isRunning; });
                now = std::chrono::steady_clock::now();
            }

            {
                std::lock_guard<std::mutex> lock(mtx);
                receivedMessage = std::to_string(i);
                cv.notify_one();
            }

            std::cout << "Produce: " << receivedMessage << "\n";
        }
    }

    std::atomic<bool> isRunning = true;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    std::string receivedMessage;
};

TEST(serial, serial) {
    var comm = std::make_unique<Comm>();

    var sa1 = std::async(std::launch::async, [&comm] { return comm->receive(); });
    var sa2 = std::async(std::launch::async, [&comm] { return comm->receive(); });

    var s1 = nexus::await | sa1;
    std::cout << "s1: " << s1.to_string() << std::endl;

    var s2 = nexus::await | sa2;
    std::cout << "s2: " << s2.to_string() << std::endl;

    EXPECT_TRUE((s1.to_string() == "0" and s2.to_string() == "1") or (s2.to_string() == "0" and s1.to_string() == "1"));

    val s3 = comm->receive([] (std::string_view text) { return text == "4"; });
    std::cout << "s3: " << s3.to_string() << std::endl;
    EXPECT_EQ(s3.to_string(), "4");

    val s4 = comm->post("receive", "");
    val json = nexus::tools::json_parse(s4);
    EXPECT_EQ(json["res"].dump(), "[53]"); // ord of '5'
}

extern "C" int c_serial();

TEST(serial, c_serial) {
    EXPECT_EQ(c_serial(), 0);
}