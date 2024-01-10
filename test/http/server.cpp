#include "gtest/gtest.h"
#include "nexus/abstract/listener.h"
#include "nexus/http/server.h"
#include "nexus/http/client.h"
#include "nexus/tools/json.h"
#include "nexus/tools/await.h"
#include <etl/keywords.h>

using namespace std::literals;

class Device : virtual public nexus::abstract::Device {
public:
    std::string json() const override { return nexus::tools::jsonify("x", x); }

protected:
    int x = 0;
};

class A : virtual public Device {
public:
    std::string path() const override { return "/a"; }
    void update() override { ++x; }
};

class B : virtual public Device {
public:
    std::string path() const override { return "/b"; }
    void update() override { --x; }
};

TEST(Server, server) {
    val host = std::string("localhost");
    val port = 5000;

    var a = A(); 
    var b = B();

    var server = nexus::http::Server();
    server.add(a).add(b);
    server.Get("/test", [] (const httplib::Request&, httplib::Response& response) {
        response.status = 200;
        response.set_content("{\"msg\": \"echo test\"}", "application/json");
    });
    server.set_logger([] (const httplib::Request& request, const httplib::Response& response) {
        std::cout << 
            request.remote_addr << " " << 
            request.method << " " << 
            request.path << " " << 
            request.version << " " << 
            response.status << std::endl;
    });

    var res = std::async(std::launch::async, [&server, host, port] { return server.listen(host, port); });
    std::cout << "Server is running on http://" << host << ":" << port << std::endl;

    std::this_thread::sleep_for(1s);
    a.update();
    b.update();

    var client = nexus::http::Client({.host=host, .port=port});
    val ra = client.Get("/a");
    val rb = client.Get("/b");
    val rt = client.Get("/test");

    val ja = nexus::tools::json_parse(ra->body);
    val jb = nexus::tools::json_parse(rb->body);
    val jt = nexus::tools::json_parse(rt->body);

    EXPECT_EQ(ja["x"].to_int(), 1);
    EXPECT_EQ(jb["x"].to_int(), -1);
    EXPECT_EQ(jt["msg"].to_string(), "echo test");

    server.stop();
    std::cout << (nexus::await | res ? "Server stop" : "Server fail to start in the first place") << std::endl;
}


TEST(Server, listener) {
    var listener = nexus::abstract::Listener();
    listener
        .add(std::make_unique<A>())
        .add(std::make_unique<B>());
    
    std::this_thread::sleep_for(1s);

    val sa = listener[0].json();
    val sb = listener[1].json();

    std::cout << sa << sb << std::endl;

    val ja = nexus::tools::json_parse(sa);
    val jb = nexus::tools::json_parse(sb);

    EXPECT_EQ(listener[0].path(), "/a");
    EXPECT_EQ(listener[1].path(), "/b");
    EXPECT_GT(ja["x"].to_int(), 0);
    EXPECT_LT(jb["x"].to_int(), 0);
}

extern "C" int c_http_server();

TEST(Server, cserver) {
    EXPECT_EQ(c_http_server(), 0);
}