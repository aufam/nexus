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

class FS50L : virtual public nexus::modbus::rtu::Client, virtual public nexus::abstract::Device {
public:
    FS50L(int address = 0x01, std::string port = "auto") : nexus::modbus::rtu::Client(address, port, B9600) {}
    virtual ~FS50L() {}

    void update() override {
        auto res = this->ReadHoldingRegisters(0x3001, 7);
        if (error() == nexus::modbus::Error::NONE) {
            frequencyRunning = res[0];
            busVoltage = res[1] * .1f;
            outputVoltage = res[2] * .1f;
            outputCurrent = res[3];
            outputPower = res[4];
            outputTorque = res[5];
            runSpeed = res[6];
        } else {
            frequencyRunning = NAN;
            busVoltage = NAN;
            outputVoltage = NAN;
            outputCurrent = NAN;
            outputPower = NAN;
            outputTorque = NAN;
            runSpeed = NAN;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        auto res2 = this->ReadHoldingRegisters(0x8000, 1);
        if (error() == nexus::modbus::Error::NONE) {
            faultInfo = res2[0];
        } else {
            faultInfo = -1;
        }
    }

    std::string path() const override { 
        return "/fs50l"; 
    }
    
    std::string json() const override { 
        return nexus::tools::json_concat(
            nexus::modbus::rtu::Client::json(), 
            "{"
                "\"frequencyRunning\": " + nexus::tools::to_string(frequencyRunning, 2) + ", "
                "\"busVoltage\": " + nexus::tools::to_string(busVoltage, 1) + ", "
                "\"outputCurrent\": " + nexus::tools::to_string(outputCurrent, 1) + ", "
                "\"outputPower\": " + nexus::tools::to_string(outputPower, 1) + ", "
                "\"outputTorque\": " + nexus::tools::to_string(outputTorque, 1) + ", "
                "\"runSpeed\": " + nexus::tools::to_string(runSpeed, 2) + ", "
                "\"faultInfo\": " + std::to_string(faultInfo) + 
            "}"
        ); 
    }

    std::string post(std::string_view method_name, std::string_view json_request) override {
        if (method_name == "forward_running") {
            this->WriteSingleRegister(0x1000, 0x0001);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("forward running")
                : nexus::tools::json_response_status_fail("forward running");
        }

        if (method_name == "reverse_running") {
            this->WriteSingleRegister(0x1000, 0x0002);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("reverse running")
                : nexus::tools::json_response_status_fail("reverse running");
        }

        if (method_name == "forward_jog") {
            this->WriteSingleRegister(0x1000, 0x0003);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("forward jog")
                : nexus::tools::json_response_status_fail("forward jog");
        }

        if (method_name == "reverse_jog") {
            this->WriteSingleRegister(0x1000, 0x0004);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("reverse jog")
                : nexus::tools::json_response_status_fail("reverse jog");
        }

        if (method_name == "free_stop") {
            this->WriteSingleRegister(0x1000, 0x0005);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("free stop")
                : nexus::tools::json_response_status_fail("free stop");
        }

        if (method_name == "decelerate_stop") {
            this->WriteSingleRegister(0x1000, 0x0006);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("decelerate stop")
                : nexus::tools::json_response_status_fail("decelerate stop");
        }

        if (method_name == "fault_resetting") {
            this->WriteSingleRegister(0x1000, 0x0007);
            return error() == nexus::modbus::Error::NONE
                ? nexus::tools::json_response_status_success("fault resetting")
                : nexus::tools::json_response_status_fail("fault resetting");
        }
        
        return nexus::modbus::rtu::Client::post(method_name, json_request);
    }

    std::string patch(std::string_view json_request) override {
        return nexus::modbus::rtu::Client::patch(json_request);
    }

    float frequencyRunning;
    float busVoltage;
    float outputVoltage;
    float outputCurrent;
    float outputPower;
    float outputTorque;
    float runSpeed;
    int faultInfo;
};

int main(int argc, char* argv[]) {
    var serial_port = std::string("auto");
    var device_address = 0x01;
    var host = std::string("localhost");
    var port = 5000;
    var path_source_pairs = std::unordered_map<std::string, std::string>({
        {"/", std::string(nexus::tools::parent_path(__FILE__) / "fs50l.html")}
    });

    nexus::tools::execute_options(argc, argv, {
        {'s', "serial-port", required_argument, [&] (const char* arg) { 
            serial_port = arg; 
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
        {'h', "help", no_argument, [] (const char*) {
            std::cout << "FS50L Interface\n";
            std::cout << "Options:\n";
            std::cout << "-s, --serial-port     Specify the serial port. Default = auto\n";
            std::cout << "-d, --device-address  Specify the device address. Default = 1\n";
            std::cout << "-H, --host            Specify the server host. Default = localhost\n";
            std::cout << "-p, --port            Specify the server port. Default = 5000\n";
            std::cout << "-P, --page            Specify the HTML page to serve in the main path. Default = fs50l.html\n";
            std::cout << "-f, --path-file       Specify the additional path-file pair. eg: /src.js:static/src.js\n";
            std::cout << "-h, --help            Print help\n";
            exit(0);
        }},
    });
    
    var fs50l = std::make_shared<FS50L>(device_address, serial_port);

    var listener = nexus::abstract::Listener();
    listener.interval = 1s;
    listener.add(fs50l);

    var server = nexus::http::Server();
    server.add(fs50l);

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
