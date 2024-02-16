#include "nexus/abstract/listener.h"
#include "nexus/tools/json.h"
#include "nexus/tools/check_index.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace Project::nexus;
using namespace std::literals;

abstract::Listener::Listener() {
    worker = std::thread(&Listener::work, this);
}

abstract::Listener::~Listener() {
    stop();
}

fun abstract::Listener::stop() -> void {
    isRunning = false;
    if (worker.joinable())
        worker.join();
}

fun abstract::Listener::json() const -> std::string {
    std::string json_devices = "[";

    if (len() > 0) {
        for (size_t i = 0; i < len(); ++i) {
            json_devices += devices[i]->json() + ", ";
        }
        json_devices.pop_back();
        json_devices.back() = ']';
    }
    else {
        json_devices = "[]";
    }
    
    return "{"
        "\"length\": " + std::to_string(len()) + ", "
        "\"devices\": " + json_devices +
    "}";
}

fun abstract::Listener::post(std::string_view method_name, std::string_view json_request) -> std::string {
    try {
        if (method_name != "remove") 
            throw std::invalid_argument("Unknown method");
        
        val index = tools::json_parse(json_request)["index"];
        if (not index)
            throw std::invalid_argument("Invalid key");

        if (not index.is_number()) 
            throw std::invalid_argument("Invalid value type");
        
        remove(index.to_int());
        return tools::json_response_status_success("Successfully remove item[" + std::to_string(index.to_int()) + "]");
    } 
    catch (const std::exception& e) {
        return tools::json_response_status_fail(e.what());
    }
}

fun abstract::Listener::work() -> void {
    for (var now = std::chrono::steady_clock::now(); isRunning;) {
        std::unique_lock<std::mutex> lock(mutex);
        
        cv.wait_until(lock, now + interval, [this] { return not isRunning; });
        now = std::chrono::steady_clock::now();

        for (var &device in devices)
            device->update();
    }
}

fun abstract::Listener::add(std::shared_ptr<Device> device) -> Listener& {
    std::scoped_lock<std::mutex> lock(mutex);
    devices.push_back(std::move(device));
    return *this;
}

fun abstract::Listener::remove(int index) -> Listener& {
    std::scoped_lock<std::mutex> lock(mutex);
    tools::check_index(devices, index);
    return (devices.erase(devices.begin() + index), *this); 
}

fun abstract::Listener::operator[](int index) const -> const Device& {
    tools::check_index(devices, index);
    return *devices[index]; 
}

fun abstract::Listener::operator[](int index) -> Device& {
    tools::check_index(devices, index);
    return *devices[index]; 
}

fun abstract::Listener::begin() const -> iterator<const Device> {
    return { devices, 0 };
}

fun abstract::Listener::end() const -> iterator<const Device> {
    return { devices, (int) devices.size() };
}

fun abstract::Listener::begin() -> iterator<Device> {
    return { devices, 0 };
}

fun abstract::Listener::end() -> iterator<Device> {
    return { devices, (int) len() };
}

abstract::c_wrapper::Listener::~Listener() { 
    delete restful; 
}

fun abstract::c_wrapper::Listener::path() const -> std::string { 
    return restful ? restful->path() : abstract::Listener::path(); 
}

fun abstract::c_wrapper::Listener::json() const -> std::string { 
    return restful ? restful->json() : abstract::Listener::json(); 
}

fun abstract::c_wrapper::Listener::post(std::string_view method_name, std::string_view json_request) -> std::string {
    return restful ? restful->post(method_name, json_request) : abstract::Listener::post(method_name, json_request);
}

fun abstract::c_wrapper::Listener::patch(std::string_view json_request) -> std::string {
    return restful ? restful->patch(json_request) : abstract::Listener::patch(json_request);
}

extern "C" {
    typedef void* nexus_listener_t;
    typedef void* nexus_device_t;
    typedef void* nexus_restful_t;

    fun static cast(nexus_listener_t listener) { 
        return static_cast<abstract::Listener*>(listener); 
    }

    fun nexus_listener_new() -> nexus_listener_t {
        return new abstract::Listener();
    }

    fun nexus_listener_override_new(nexus_restful_t restful, void* members) -> nexus_listener_t {
        var res = new abstract::c_wrapper::Listener();
        res->restful = static_cast<abstract::Restful*>(restful);
        res->c_members = members;
        return res;
    }

    fun nexus_listener_override_delete(nexus_listener_t listener) -> void {
        delete cast(listener);
    }

    fun nexus_listener_add(nexus_listener_t listener, nexus_device_t device) -> void {
        cast(listener)->add(std::shared_ptr<abstract::Device>(static_cast<nexus::abstract::Device*>(device)));
    }

    fun nexus_listener_remove(nexus_listener_t listener, int index) -> void {
        cast(listener)->remove(index);
    }

    fun nexus_listener_len(nexus_listener_t listener) -> size_t {
        return cast(listener)->len();
    }

    fun nexus_listener_get_device(nexus_listener_t listener, int index) -> nexus_device_t {
        return &cast(listener)->operator[](index);
    }
}
