#include "nexus/abstract/device.h"
#include "nexus/abstract/listener.h"
#include "nexus/modbus/rtu/client.h"
#include "nexus/http/server.h"

#include "nexus/tools/filesystem.h"
#include "nexus/tools/options.h"
#include "nexus/tools/on_kill.h"
#include "nexus/tools/timestamp.h"
#include "nexus/tools/to_string.h"
#include "nexus/tools/json.h"

#include <iostream>
#include <etl/keywords.h>

using namespace std::literals;

class URM15 : virtual public nexus::modbus::rtu::Client, virtual public nexus::abstract::Device {
    int verbose;
    float distance = NAN;
    float temperature = NAN;

public:
    URM15(int address = 0x0F, std::string port = "auto", speed_t speed = B9600, int verbose = 0) 
        : nexus::modbus::rtu::Client(address, port, speed), verbose(verbose)
    {}

    virtual ~URM15() {}

    void update() override {
        this->WriteSingleRegister(0x0008, 0b1101); // send trigger

        std::this_thread::sleep_for(65ms);
        var res = this->ReadHoldingRegisters(0x0005, 2);

        distance = error() or res[0] == 0xFFFF ? NAN : res[0] * 0.1f;
        temperature = error() or res[1] == 0xFFFF ? NAN : res[1] * 0.1f;

        if (verbose) error() ? 
            printf("Error code: %d\n", error()) :
            printf("Distance = %.1f cm, Temperature = %.1f C\n", distance, temperature);
    }

    std::string path() const override { 
        return "/urm15"; 
    }
    
    std::string json() const override { 
        return nexus::tools::json_concat(
            nexus::modbus::rtu::Client::json(), "{"
                "\"distance\": " + nexus::tools::to_string(distance, 1) + ", "
                "\"temperature\": " + nexus::tools::to_string(temperature, 1) + 
            "}"
        ); 
    }

    std::string post(std::string_view method_name, std::string_view json_request) override {
        return nexus::modbus::rtu::Client::post(method_name, json_request);
    }

    std::string patch(std::string_view json_request) override {
        return nexus::modbus::rtu::Client::patch(json_request);
    }
};

int main(int argc, char* argv[]) {
    var serial_port = std::string("auto");
    var device_address = 0x0F;
    var host = std::string("localhost");
    var port = 5000;
    var baud = 0;
    var do_setup = 0;
    var verbose = 0;
    var path_source_pairs = std::unordered_map<std::string, std::string>({
        {"/", std::string(nexus::tools::parent_path(__FILE__) / "urm15.html")}
    });

    nexus::tools::execute_options(argc, argv, {
        {'s', "serial-port", required_argument, [&] (const char* arg) { 
            serial_port = arg; 
        }},
        {'b', "baud", required_argument, [&] (const char* arg) { 
            if (std::string("115200") == arg) baud = B115200;
            if (std::string("57600") == arg) baud = B57600;
            if (std::string("38400") == arg) baud = B38400;
            if (std::string("19200") == arg) baud = B19200;
            if (std::string("9600") == arg) baud = B9600;
            if (std::string("2400") == arg) baud = B2400;
        }},
        {'d', "device-address", required_argument, [&] (const char* arg) { 
            device_address = std::atoi(arg); 
        }},
        {'H', "host", required_argument, [&] (const char* arg) { 
            host = arg; 
        }},
        {'p', "port", required_argument, [&] (const char* arg) { 
            port = std::atoi(arg); 
        }},
        {'P', "page", required_argument, [&] (const char* arg) { 
            path_source_pairs["/"] = arg; 
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
        {'i', "do-setup", required_argument, [&] (const char* arg) { 
            do_setup = std::atoi(arg); 
        }},
        {'v', "verbose", required_argument, [&] (const char* arg) { 
            verbose = std::atoi(arg); 
        }},
        {'h', "help", no_argument, [] (const char*) {
            std::cout << "URM15 Interface\n";
            std::cout << "Options:\n";
            std::cout << "-s, --serial-port     Specify the serial port. Default = auto\n";
            std::cout << "-b, --baud            Specify the serial baud rate.  Default = 9600 or 19200 (setup mode)\n";
            std::cout << "-d, --device-address  Specify the device address. Default = 15\n";
            std::cout << "-H, --host            Specify the server host. Default = localhost\n";
            std::cout << "-p, --port            Specify the server port. Default = 5000\n";
            std::cout << "-P, --page            Specify the HTML page to serve in the main path. Default = urm15.html\n";
            std::cout << "-f, --path-file       Specify the additional path-file pair. eg: /src.js:static/src.js\n";
            std::cout << "-i, --do-setup,       Setup the device. Default = 0\n";
            std::cout << "-v, --verbose,        Verbosity level. Default = 0\n";
            std::cout << "-h, --help            Print help\n";
            exit(0);
        }},
    });

    if (do_setup) {
        var device = nexus::modbus::rtu::Client(0x00, serial_port, baud == 0 ? B19200 : baud);
        if (not device.isConnected()) {
            std::cout << "No device connected\n";
            return 0;
        }

        device.WriteSingleRegister(0x0008, 0b0101);
        std::cout << "Set control register\n";

        device.WriteSingleRegister(0x0003, 0x0003);
        std::cout << "Set baud rate to 9600\n";

        device.WriteSingleRegister(0x0002, device_address);
        std::cout << "Set address to " << device_address << "\n";

        std::cout << "Please restart the device\n";
        return 0;
    }
    
    var urm15 = std::make_shared<URM15>(device_address, serial_port, baud, verbose);

    var listener = nexus::abstract::Listener();
    listener.interval = 1s;
    listener.add(urm15);

    var server = nexus::http::Server();
    server.add(urm15);

    server.set_logger([] (const httplib::Request& request, const httplib::Response& response) { 
        std::cout << 
            nexus::tools::current_time.get() << " " << 
            request.remote_addr << " " << 
            request.method << " " << 
            request.path << " " << 
            request.version << " " << 
            response.status << std::endl;
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