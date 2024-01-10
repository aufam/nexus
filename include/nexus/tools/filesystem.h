#ifndef PROJECT_NEXUS_TOOLS_FILESYSTEM_H
#define PROJECT_NEXUS_TOOLS_FILESYSTEM_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace Project::nexus::tools {
    inline auto path(const char* __file__) {
        return std::filesystem::absolute(__file__);
    }

    inline auto parent_path(const char* __file__) {
        return std::filesystem::absolute(__file__).parent_path();
    }

    inline auto read_file(std::string file) {
        std::ifstream f(file);    
        
        if (!f.is_open()) {
            if (f.fail()) {
                // General file opening error
                throw std::runtime_error("Error opening file: " + file);
            } else if (f.eof()) {
                // file not found error
                throw std::runtime_error("File not found: " + file);
            }
        }

        std::ostringstream oss;
        oss << f.rdbuf();
        f.close();

        return oss.str();
    }
}

#endif