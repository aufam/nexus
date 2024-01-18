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
#include <etl/keywords.h>

using namespace std::literals;

class PZEM : virtual public nexus::modbus::rtu::Client, virtual public nexus::abstract::Device {
public:
    PZEM(std::string port=nexus::modbus::rtu::Client::Default::port, int address = 0xF8) 
        : nexus::modbus::rtu::Client(port, B9600) 
        , address(address) 
        {}
    
    virtual ~PZEM() {}

    void update() override {
        auto [res, err] = this->ReadInputRegisters(address, 0x0000, 10);
        if (err == nexus::modbus::Error::NONE) {
            voltage = res[0] * .1f;
            current = (res[1] | res[2] << 16) * .001f;
            power = (res[3] | res[4] << 16) * .1f;
            energy = (res[5] | res[6] << 16) * 1.f;
            frequency = res[7] * .1f;
            powerFactor = res[8] * .01f;
            alarm = res[9] == 0xFFFF;
        } else {
            voltage = NAN;
            current = NAN;
            power = NAN;
            energy = NAN;
            frequency = NAN;
            powerFactor = NAN;
            alarm = false;
        }

        auto [res2, err2] = this->ReadHoldingRegisters(address, 0x0001, 1);
        if (err2 == nexus::modbus::Error::NONE) {
            alarmThreshold = res2[0];
        } else {
            alarmThreshold = NAN;
        }
    }
    
    std::string path() const override { 
        return "/pzem-004t"; 
    }
    
    std::string json() const override { 
        return nexus::tools::json_concat(
            nexus::serial::Serial::json(), 
            "{"
                "\"address\": " + std::to_string(address) + ", "
                "\"voltage\": " + nexus::tools::to_string(voltage, 1) + ", "
                "\"current\": " + nexus::tools::to_string(current, 3) + ", "
                "\"power\": " + nexus::tools::to_string(power, 1) + ", "
                "\"energy\": " + nexus::tools::to_string(energy, 1) + ", "
                "\"frequency\": " + nexus::tools::to_string(frequency, 1) + ", "
                "\"powerFactor\": " + nexus::tools::to_string(powerFactor, 2) + ", "
                "\"alarm\": " + std::string(alarm ? "true" : "false") + ", "
                "\"alarmThreshold\": " + nexus::tools::to_string(alarmThreshold, 1) +
            "}"
        ); 
    }

    std::string post(std::string_view method_name, std::string_view json_request) override {
        if (method_name == "reset_energy") {
            uint8_t buf[] = {uint8_t(address), 0x42};
            this->Request(buf);
            return nexus::tools::json_response_status_success("request to reset energy");
        }

        if (method_name == "calibrate") {
            uint8_t buf[] = {uint8_t(address), 0x41, 0x37, 0x21};
            this->Request(buf);
            return nexus::tools::json_response_status_success("request to calibrate");
        }

        return nexus::modbus::rtu::Client::post(method_name, json_request);
    }

    std::string patch(std::string_view json_string) override {
        auto response = std::string();
        auto json = nexus::tools::json_parse(json_string);
        auto ad = json["address"];
        auto at = json["alarmThreshold"];

        if (ad.is_number()) {
            auto [res, err] = this->WriteSingleRegister(address, 0x0001, ad.to_int());
            if (err == nexus::modbus::Error::NONE) {
                address = res;
            }
            response = nexus::tools::json_concat(response, "\"address\": " + std::to_string(address) + "}");
        }
        
        if (at.is_number()) {
            auto [res, err] = this->WriteSingleRegister(address, 0x0002, at.to_int());
            if (err == nexus::modbus::Error::NONE) {
                alarmThreshold = res;
            }
            response = nexus::tools::json_concat(response, "\"alarmThreshold\": " + std::to_string(alarmThreshold) + "}");
        }

        return response.empty() 
            ? nexus::tools::json_response_status_fail("Unknown key")
            : nexus::tools::json_concat(nexus::tools::json_response_status_success("success update parameter"), response);
    }

    int address;
    float voltage;
    float current;
    float power;
    float energy;
    float frequency;
    float powerFactor;
    bool alarm;
    float alarmThreshold;
};

int main(int argc, char* argv[]) {
    var serial_port = std::string("auto");
    var device_address = 0xF8;
    var host = std::string("localhost");
    var port = 5000;
    var page = std::string(nexus::tools::parent_path(__FILE__) / "pzem-004t.html");

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
            page = arg; 
        }},
        {'h', "help", no_argument, [] (const char*) {
            std::cout << "PZEM Interface\n";
            std::cout << "Options:\n";
            std::cout << "-s, --serial-port     Specify the serial port. Default = auto\n";
            std::cout << "-d, --device-address  Specify the device address. Default = 1\n";
            std::cout << "-H, --host            Specify the server host. Default = localhost\n";
            std::cout << "-p, --port            Specify the server port. Default = 5000\n";
            std::cout << "-P, --page            Specify the HTML page to serve in the main path. Default = pzem-004t.index\n";
            std::cout << "-h, --help            Print help\n";
            exit(0);
        }},
    });
    
    var listener = nexus::abstract::Listener();
    listener.interval = 1s;
    listener.add(std::make_unique<PZEM>(serial_port, device_address));

    var server = nexus::http::Server();
    server.add(listener[0]);
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

    nexus::tools::on_kill([&server] { server.stop(); });

    std::cout << "Server is running on http://" << host << ":" << port << "/" << std::endl;
    server.listen(host, port);
    std::cout << std::endl;
    return 0;
}