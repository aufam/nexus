#include "nexus/http/server.h"
#include "nexus/abstract/listener.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

using namespace Project::nexus;

fun static set_response_from_get(httplib::Response& response, const std::string& json_string) {
    response.status = 200; 
    response.set_content(json_string, "application/json");
}

fun static set_response(httplib::Response& response, const std::string& json_string) {
    response.status = tools::json_response_get_status(json_string); 
    response.set_content(json_string, "application/json");
}

http::Server::Server() {}

fun http::Server::add(RestfulHandler args) -> Server& {
    if (handlers.end() != std::find_if(handlers.begin(), handlers.end(), [&args] (const RestfulHandler& handler) {
        return handler.restful == args.restful;
    })) {
        return *this;
    }

    if (args.query.count("index") == 0) {
        int index = 0;
        std::list<int> indices;
        for (var &handler in handlers) if (args.base_path + args.restful->path() == handler.base_path + handler.restful->path()) {
            indices.push_back(std::atoi(handler.query["index"].c_str()));
        }

        indices.sort();

        for (val i in indices) {
            if (i == index) ++index;
            else break;
        }

        args.query["index"] = std::to_string(index);
    }

    handlers.push_back(args);
    
    this->Get(args.base_path + args.restful->path(), [this] (const httplib::Request& request, httplib::Response& response) {
        var res = find_restful(request);
        if (not res.empty())
            set_response_from_get(response, res.front()->json());
        else
            response.status = 404;
    });

    this->Patch(args.base_path + args.restful->path(), [this] (const httplib::Request& request, httplib::Response& response) {
        var res = find_restful(request);
        if (not res.empty())
            set_response(response, res.front()->patch(request.body));
        else
            response.status = 404;
    });

    this->Post(args.base_path + args.restful->path(), [this] (const httplib::Request& request, httplib::Response& response) {
        if (not request.has_param("method")) {
            set_response(response, tools::json_response_status_fail("Query method is not specified"));
            return;
        }
        if (request.get_param_value_count("method") > 1) {
            set_response(response, tools::json_response_status_fail("Multiple query methods"));
            return;
        }

        var res = find_restful(request);
        if (not res.empty())
            set_response(response, res.front()->post(request.get_param_value("method"), request.body));
        else
            response.status = 404;
    });

    this->Delete(args.base_path + args.restful->path(), [this] (const httplib::Request& request, httplib::Response& response) {
        var res = find_restful(request);
        if (res.empty()) {
            response.status = 404;
            return;
        }
        
        for (var &restful in res) {
            handlers.remove_if([&restful] (const RestfulHandler& handler) { return handler.restful == restful; });
        }
        set_response(response, tools::json_response_status_success(request.path));
    });

    var listener = dynamic_cast<abstract::Listener*>(args.restful.get());
    if (listener == nullptr)
        return *this;
    
    for (var &restful in *listener) add(RestfulHandler{
        .restful=std::shared_ptr<abstract::Restful>(&restful, [restful=args.restful] (abstract::Restful* ptr) {
            var listener = dynamic_cast<abstract::Listener*>(restful.get());
            for (size_t i = 0; i < listener->len(); ++i) {
                if (ptr == &listener->operator[](i))
                    listener->remove(i);
            }
        }),
        .base_path=args.base_path + listener->path()
    });

    return *this;
}

fun http::Server::find_restful(const httplib::Request& request, bool all) const -> std::list<std::shared_ptr<abstract::Restful>> {
    var res = std::list<std::shared_ptr<abstract::Restful>>();

    var m = std::unordered_map<std::string, std::list<std::string>>();
    for (val &[key, value] in request.params) if (key != "method")
        m[key].push_back(value);
    
    for (val &handler in handlers) {
        if (not all and request.path != handler.base_path + handler.restful->path())
            continue;

        bool found = true;
        for (val &[key, values] in m) {
            var it = handler.query.find(key);

            std::string sv;
            if (it == handler.query.end()) {
                // try to find it in json
                val json = tools::json_parse(handler.restful->json())[etl::string_view(key.c_str())];
                if (not json) {
                    found = false;
                    break;
                }
                var s = json.is_string() ? json.to_string() : json.dump();
                sv = std::string(s.data(), s.len());
            } else {
                sv = it->second;
            }

            if (std::find(values.begin(), values.end(), sv) == values.end()) {
                found = false;
                break;
            }
        }

        if (found)
            res.push_back(handler.restful);
    }

    return res;
};

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

fun nexus_http_server_add_restful(nexus_http_server_t server, 
    nexus_restful_t restful,
    const char* base_path, 
    const char** query_keys, 
    const char** query_values, 
    size_t query_len
) -> void {
    var res = std::shared_ptr<nexus::abstract::Restful>(
        static_cast<nexus::abstract::Restful*>(restful), 
        [] (nexus::abstract::Restful*) {}
    );

    var query = std::unordered_map<std::string, std::string>();
    for (size_t i = 0; i < query_len; ++i)
        query[query_keys[i]] = query_values[i];
    
    cast_server(server)->add(res, base_path, std::move(query));
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