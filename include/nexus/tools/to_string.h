#ifndef PROJECT_NEXUS_TOOLS_TO_STRING_H
#define PROJECT_NEXUS_TOOLS_TO_STRING_H

#include <string>
#include <cmath>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <etl/string_view.h>

namespace Project::nexus::tools {

    inline std::string to_string(float value, int precision=3) {
        if (std::isnan(value))
            return "NaN";
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }

    inline std::string to_string(double value, int precision=3) {
        return to_string(float(value), precision);
    }

    inline std::string to_string(bool value) {
        return value ? "true" : "false";
    }

    inline std::string to_string(int value) {
        return std::to_string(value);
    }

    inline std::string to_string(std::string_view str) {
        return "\"" + std::string(str) + "\"";
    }

    inline std::string to_string(etl::StringView str) {
        return to_string(std::string_view(str.data(), str.len()));
    }

    template <typename T>
    std::string to_string(const std::vector<T>& vector) {
        if (vector.empty())
            return "[]";

        std::string res("[");
        for (auto &value : vector) {
            res += to_string(value) + ", ";
        }
        
        res.pop_back();
        res.back() = ']';
        return res;
    }

    template <typename T>
    std::string to_string(const std::function<T()>& fn) {
        return to_string(fn());
    }

    template <typename K, typename V>
    std::string to_string(const std::unordered_map<K, V>& map) {
        if (map.empty())
            return "{}";

        std::string res("{");
        for (auto &[key, value] : map) {
            auto key_str = to_string(key);
            if constexpr (std::is_integral_v<K>) {
                key_str = to_string(key_str);
            }

            res += key_str + ": " + to_string(value) + ", ";
        }
        
        res.pop_back();
        res.back() = '}';
        return res;
    }
}

#endif
