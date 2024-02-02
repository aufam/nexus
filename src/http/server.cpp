#include "nexus/http/server.h"
#include "nexus/abstract/listener.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

using namespace Project::nexus;

http::Server::Server() {}

fun static set_response_from_get(httplib::Response& response, const std::string& json_string) {
    response.status = 200; 
    response.set_content(json_string, "application/json");
}

fun static set_response(httplib::Response& response, const std::string& json_string) {
    response.status = tools::json_response_get_status(json_string); 
    response.set_content(json_string, "application/json");
}

fun http::Server::add(abstract::Restful& restful, int index) -> Server& {
    std::string pattern = restful.path();
    if (index >= 0) pattern += "/" + std::to_string(index);

    this->Get(pattern, [this, &restful] (const httplib::Request&, httplib::Response& response) {
        set_response_from_get(response, restful.json());
    });

    this->Patch(pattern, [this, &restful] (const httplib::Request& request, httplib::Response& response) {
        set_response(response, restful.patch(request.body));
    });

    this->Post(pattern + "/(.*)", [this, &restful] (const httplib::Request& request, httplib::Response& response) {
        std::string method = request.matches[1];
        set_response(response, restful.post(method, request.body));
    });

    var listener = dynamic_cast<abstract::Listener*>(&restful);
    if (listener == nullptr)
        return *this;

    this->Get(pattern + R"(/(.*)/(\d+))", [this, listener] (const httplib::Request& request, httplib::Response& response) {
        try {
            val devicePath = "/" + std::string(request.matches[1]);
            val deviceIndex = std::stoi(std::string(request.matches[2]));
            val &device = listener->operator[](deviceIndex);

            if (devicePath == device.path()) 
                set_response_from_get(response, device.json());
            
        } catch (const std::exception& e) {
            set_response(response, tools::json_response_status_fail(e.what()));
        }
    });

    this->Patch(pattern + R"(/(.*)/(\d+))", [this, listener] (const httplib::Request& request, httplib::Response& response) {
        try {
            val devicePath = "/" + std::string(request.matches[1]);
            val deviceIndex = std::stoi(std::string(request.matches[2]));
            var &device = listener->operator[](deviceIndex);

            if (devicePath == device.path()) 
                set_response(response, device.patch(request.body));
            
        } catch (const std::exception& e) {
            set_response(response, tools::json_response_status_fail(e.what()));
        }
    });

    this->Post(pattern + R"(/(.*)/(\d+)/(.*))", [this, listener] (const httplib::Request& request, httplib::Response& response) {
        try {
            val devicePath = "/" + std::string(request.matches[1]);
            val deviceIndex = std::stoi(std::string(request.matches[2]));
            val deviceMethod = std::string(request.matches[3]);
            var &device = listener->operator[](deviceIndex);

            if (devicePath == device.path()) 
                set_response(response, device.post(deviceMethod, request.body));
            
        } catch (const std::exception& e) {
            set_response(response, tools::json_response_status_fail(e.what()));
        }
    });

    this->Delete(pattern + R"(/(.*)/(\d+))", [this, listener] (const httplib::Request& request, httplib::Response& response) {
        try {
            val devicePath = "/" + std::string(request.matches[1]);
            val deviceIndex = std::stoi(std::string(request.matches[2]));
            var &device = listener->operator[](deviceIndex);

            if (devicePath == device.path()) {
                listener->remove(deviceIndex);
                response.status = 204;
                response.reason = "No Content";
            }
            
        } catch (const std::exception& e) {
            set_response(response, tools::json_response_status_fail(e.what()));
        }
    });

    return *this;
}

extern "C" {

typedef void* nexus_restful_t;
typedef void* nexus_http_server_t;
typedef const void* nexus_http_request_t;
typedef void* nexus_http_response_t;
typedef void (*nexus_http_server_handler_t)(nexus_http_request_t request, nexus_http_response_t response);

fun static cast_server(nexus_http_server_t server) {
    return static_cast<nexus::http::Server*>(server);
}

fun nexus_http_server_new() -> nexus_http_server_t {
    return new nexus::http::Server();
}

fun nexus_http_server_delete(nexus_http_server_t server) -> void {
    delete cast_server(server);
}

fun nexus_http_server_add_method(nexus_http_server_t server, const char* method, const char* path, nexus_http_server_handler_t handler) -> void {
    std::string_view mtd = method;
    if (mtd == "GET") {
        cast_server(server)->Get(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    } elif (mtd == "POST") {
        cast_server(server)->Post(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    } elif (mtd == "PUT") {
        cast_server(server)->Put(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    } elif (mtd == "PATCH") {
        cast_server(server)->Patch(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    } elif (mtd == "DELETE") {
        cast_server(server)->Delete(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    } elif (mtd == "OPTIONS") {
        cast_server(server)->Options(path, [handler] (const httplib::Request& request, httplib::Response& response) {
            handler(&request, &response);
        });
    }
}

fun nexus_http_server_add_restful(nexus_http_server_t server, nexus_restful_t restful) -> void {
    cast_server(server)->add(*static_cast<nexus::abstract::Restful*>(restful));
}

fun nexus_http_server_add_restful_with_index(nexus_http_server_t server, nexus_restful_t restful, int index) -> void {
    cast_server(server)->add(*static_cast<nexus::abstract::Restful*>(restful), index);
}

fun nexus_http_server_listen(nexus_http_server_t server, const char* host, int port) -> void {
    cast_server(server)->listen(host, port);
}

fun nexus_http_server_is_running(nexus_http_server_t server) -> int {
    return cast_server(server)->is_running();
}

fun nexus_http_server_stop(nexus_http_server_t server) -> void {
    cast_server(server)->stop();
}

void nexus_http_server_set_logger(nexus_http_server_t server, nexus_http_server_handler_t handler) {
    cast_server(server)->set_logger([handler] (const httplib::Request& request, const httplib::Response& response) {
        handler(&request, const_cast<httplib::Response*>(&response));
    });
}

}