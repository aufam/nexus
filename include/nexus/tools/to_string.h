#ifndef PROJECT_NEXUS_TOOLS_TO_STRING_H
#define PROJECT_NEXUS_TOOLS_TO_STRING_H

#include <string>
#include <sstream>
#include <iomanip>

namespace Project::nexus::tools {

    inline std::string to_string(float value, int precision) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(precision) << value;
        return ss.str();
    }

    template <typename T>
    std::string to_string(const T& value) {
        return std::to_string(value);
    }
}

#endif
