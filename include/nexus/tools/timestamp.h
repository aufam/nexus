#ifndef PROJECT_NEXUS_TIMESTAMP_H
#define PROJECT_NEXUS_TIMESTAMP_H

#include <etl/getter_setter.h>
#include <string>
#include <functional>
#include <ctime>
#include <iomanip>  // For std::put_time

namespace Project::nexus::tools {
    static const etl::Getter<std::string, std::function<std::string()>> current_time = { [] {
        std::time_t currentTime = std::time(nullptr);

        std::tm* localTime = std::localtime(&currentTime);
        std::stringstream ss;
        ss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");

        return ss.str();
    }};
}


#endif