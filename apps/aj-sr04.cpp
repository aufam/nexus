#include "nexus/abstract/device.h"
#include "nexus/abstract/listener.h"
#include "nexus/serial/software.h"
#include "nexus/http/server.h"

#include "nexus/tools/filesystem.h"
#include "nexus/tools/options.h"
#include "nexus/tools/on_kill.h"
#include "nexus/tools/timestamp.h"
#include "nexus/tools/to_string.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

using namespace std::literals;

class AJSR04Codec : public nexus::abstract::Codec {
public:
    nexus::byte_view encode(nexus::byte_view buffer) const override { return buffer; }
    nexus::byte_view decode(nexus::byte_view buffer) const override {
        return buffer.slice(0, 4).to_string() == "Gap=" 
            && buffer.slice(buffer.len() - 3) == nexus::byte_view({0x0d, 0x0a, 0x0d}) 
            ? buffer.slice(4) : nexus::byte_view{};
    }
};

class AJSR04 : virtual public nexus::abstract::Device, virtual public nexus::serial::Software {
public:
    AJSR04(std::string port) : nexus::serial::Software(port, B9600, 1s, std::make_shared<AJSR04Codec>()) {}

    void update() override {
        uint8_t dummy[] = {0x01};
        send(nexus::byte_view{dummy, sizeof(dummy)});
        auto response = receive();
        distance = etl::string_view(response.data(), response.size()).to_float() / 1000.f;
        std::cout << "Distance: " << distance << " m" << std::endl;
    }

    std::string path() const override { 
        return "/aj-sr04"; 
    }
    
    std::string json() const override { 
        return nexus::tools::json_concat(
            nexus::serial::Software::json(), 
            "{\"distance\": " + nexus::tools::to_string(distance, 3) + "}"
        ); 
    }

    using nexus::serial::Software::post;
    using nexus::serial::Software::patch;

    float distance;
};

int main(int argc, char* argv[]) {
    var serial_port = std::string("auto");
    var host = std::string("localhost");
    var port = 5000;
    var page = std::string(nexus::tools::parent_path(__FILE__) / "aj-sr04.html");
    var path_source_pairs = std::map<std::string, std::string>();

    nexus::tools::execute_options(argc, argv, {
        {'s', "serial-port", required_argument, [&] (const char* arg) { 
            serial_port = arg; 
        }},
        {'H', "host", required_argument, [&] (const char* arg) { 
            host = arg; 
        }},
        {'p', "port", required_argument, [&] (const char* arg) { 
            port = std::atoi(arg); 
        }},
        {'P', "page", required_argument, [&] (const char* arg) { 
            page = arg; 
        }},
        {'f', "path-file", required_argument, [&] (const char* arg) { 
            // Parse pairs of paths and source files
            std::string pair(arg);
            size_t pos = pair.find(":");
            if (pos != std::string::npos) {
                std::string path = pair.substr(0, pos);
                std::string source = pair.substr(pos + 1);
                path_source_pairs[path] = source;
            } else {
                std::cerr << "Invalid format for -f option. Use -f path,file" << std::endl;
                exit(1);
            }
        }},
        {'h', "help", no_argument, [] (const char*) {
            std::cout << "AJ-SR04M Interface\n";
            std::cout << "Options:\n";
            std::cout << "-s, --serial-port Specify the serial port. Default = auto\n";
            std::cout << "-H, --host        Specify the server host. Default = localhost\n";
            std::cout << "-p, --port        Specify the server port. Default = 5000\n";
            std::cout << "-P, --page        Specify the HTML page to serve in the main path. Default = aj-sr04.html\n";
            std::cout << "-f, --path-file   Specify the additional path-file pair. eg: /src.js:static/src.js\n";
            std::cout << "-h, --help        Print help\n";
            exit(0);
        }},
    });
    
    var ajsr04 = std::make_shared<AJSR04>(serial_port);
    var listener = nexus::abstract::Listener();
    listener.interval = 1s;
    listener.add(ajsr04);

    var server = nexus::http::Server();
    server.add(ajsr04);
    
    server.set_logger([] (const httplib::Request& request, const httplib::Response& response) { 
        std::cout << 
            nexus::tools::current_time.get() << " " << 
            request.remote_addr << " " << 
            request.method << " " << 
            request.path << " " << 
            request.version << " " << 
            response.status << std::endl;
    }); 

    server.Get("/", [&page] (const httplib::Request&, httplib::Response& response) {
        try {
            response.set_content(nexus::tools::read_file(page), "text/html");
            response.status = 200;
        } catch (const std::exception& e) {
            response.set_content(e.what(), "text/plain");
            response.status = 500;
        }
    });

    for (val &[path, source] in path_source_pairs) {
        server.Get(path, [&source] (const httplib::Request&, httplib::Response& response) {
            try {
                response.set_content(nexus::tools::read_file(source), nexus::tools::content_type(source));
                response.status = 200;
            } catch (const std::exception& e) {
                response.set_content(e.what(), "text/plain");
                response.status = 500;
            }
        });
    }

    nexus::tools::on_kill([&server] { server.stop(); });

    std::cout << "Server is running on http://" << host << ":" << port << "/" << std::endl;
    server.listen(host, port);
    std::cout << std::endl;
    return 0;
}