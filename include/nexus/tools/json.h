#ifndef PROJECT_NEXUS_TOOLS_JSON_H
#define PROJECT_NEXUS_TOOLS_JSON_H

#include <string>
#include <vector>
#include <etl/json.h>

namespace Project::nexus::tools {
    template <typename T>
    std::string jsonify(const std::string& key, T value) {
        return "{\"" + key + "\": " + std::to_string(value) + "}"; 
    }

    inline constexpr etl::Json json_parse(std::string_view json_string) {
        return etl::Json::parse(etl::string_view(json_string.data(), json_string.length()));
    }

    inline std::string json_concat(std::string_view s1, std::string_view s2) {
        auto j1 = json_parse(s1);
        auto j2 = json_parse(s2);
        
        if (!j1.is_dictionary() && !j2.is_dictionary()) {
            return std::string(); // both inputs are not valid
        } else if (!j1.is_dictionary()) {
            return std::string(s2);
        } else if (!j2.is_dictionary()) {
            return std::string(s1);
        }

        auto sv1 = j1.dump();
        auto sv2 = j2.dump();

        auto res1 = std::string(sv1.data(), sv1.len() - 1);     // remove '}'
        auto res2 = std::string(sv2.data() + 1, sv2.len() - 1); // remove '{'

        return res1 + ", " + res2; 
    }

    inline std::string json_response_status_success(const std::string& message) {
        return "{"
            "\"status\": \"success\", "
            "\"message\": \"" + message + "\""
        "}";
    }

    inline std::string json_response_status_fail(const std::string& message) {
        return "{"
            "\"status\": \"fail\", "
            "\"message\": \"" + message + "\""
        "}";
    }

    inline std::string json_response_status_warning(const std::string& message) {
        return "{"
            "\"status\": \"warning\", "
            "\"message\": \"" + message + "\""
        "}";
    }

    inline constexpr int json_response_get_status(std::string_view json_string) {
        auto json = json_parse(json_string);
        auto status = json["status"].to_string();
        return status == "success" || status == "warning" ? 200 : status == "fail" ? 500 : 404;
    }

    inline std::string json_response_status_fail_mismatch_value_type() {
        return json_response_status_fail("Value type doesn't match");
    }

    inline std::string json_response_status_fail_unknown_key() {
        return json_response_status_fail("Unknown key");
    }

    inline std::string json_response_status_fail_unknown_method() {
        return json_response_status_fail("Unknown method");
    }

    template <typename T>
    std::vector<T> json_to_vector(const etl::Json& json) {
        static_assert(
            std::is_integral_v<T> || 
            std::is_floating_point_v<T> || 
            std::is_same_v<T, bool> || 
            std::is_same_v<T, std::string>, 
        "Unsupported type");

        auto res = std::vector<T>(json.len());
        for (auto [src, dest]: etl::zip(json, res)) {
            if constexpr (std::is_integral_v<T>) {
                dest = src.to_int();
            } else if constexpr (std::is_floating_point_v<T>) {
                dest = src.to_float();
            } else if constexpr (std::is_same_v<T, bool>) {
                dest = src.is_true();
            } else if constexpr (std::is_same_v<T, std::string>) {
                dest = std::string(src.to_string().begin(), src.to_string().end());
            }
        }
        
        return res;
    }
}

#endif
