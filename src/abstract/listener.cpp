#include "nexus/abstract/listener.h"
#include "nexus/tools/json.h"
#include "nexus/tools/check_index.h"
#include "nexus/abstract/c_wrapper.h"
#include "helper.ipp"
#include <etl/keywords.h>

using namespace Project::nexus;
using namespace std::literals;

abstract::Listener::Listener() {
    worker = std::thread(&Listener::work, this);
}

abstract::Listener::~Listener() {
    isRunning = false;
    if (worker.joinable())
        worker.join();
}

fun abstract::Listener::stop() -> void {
    isRunning = false;
    worker.detach();
}

fun abstract::Listener::json() const -> std::string {
    std::string json_devices = "[";
    for (val &device in devices) {
        json_devices += device->json() + ", ";
    }
    json_devices.pop_back();
    json_devices.back() = ']';
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

fun abstract::Listener::add(std::unique_ptr<Device> device) -> Listener& {
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

cwrapper::Listener::~Listener() { delete restful; }

fun cwrapper::Listener::path() const -> std::string { 
    return get_restful_response(restful->cpath, restful->cmembers); 
}

fun cwrapper::Listener::json() const -> std::string { 
    auto res = get_restful_response(restful->cjson, restful->cmembers); 
    if (res.empty()) res = abstract::Listener::json();
    return res;
}

fun cwrapper::Listener::post(std::string_view method_name, std::string_view json_request) -> std::string {
    auto res = get_restful_response(restful->cpost, restful->cmembers, method_name.data(), json_request.data());
    if (res.empty()) res = abstract::Listener::post(method_name, json_request);
    return res;
}

fun cwrapper::Listener::patch(std::string_view json_request) -> std::string {
    return get_restful_response(restful->cpatch, restful->cmembers, json_request.data());
}

extern "C" {
    fun static cast(nexus_listener_t listener) { 
        return static_cast<nexus::abstract::Listener*>(listener); 
    }

    fun nexus_listener_new() -> nexus_listener_t {
        return new nexus::abstract::Listener();
    }

    fun nexus_listener_delete(nexus_listener_t listener) -> void {
        delete cast(listener);
    }

    fun nexus_listener_override_new(nexus_restful_t restful) -> nexus_listener_t {
        var res = new nexus::cwrapper::Listener();
        res->restful = static_cast<nexus::cwrapper::Restful*>(restful);
        return res;
    }

    fun nexus_listener_override_delete(nexus_listener_t listener) -> void {
        delete static_cast<nexus::cwrapper::Listener*>(listener);
    }

    fun nexus_listener_add(nexus_listener_t listener, nexus_device_t device) -> void {
        cast(listener)->add(std::unique_ptr<nexus::abstract::Device>(static_cast<nexus::abstract::Device*>(device)));
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
