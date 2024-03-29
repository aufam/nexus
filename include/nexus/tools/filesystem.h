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
            else {
                throw std::runtime_error("Unknow error: " + file);
            }
        }

        std::ostringstream oss;
        oss << f.rdbuf();
        f.close();

        return oss.str();
    }

    inline std::string content_type(std::string file) {
        std::string extension = file.substr(file.find_last_of('.') + 1);
        return 
            extension == "js"   ? "application/javascript" :
            extension == "pdf"  ? "application/pdf" :
            extension == "xml"  ? "application/xml" :
            extension == "css"  ? "text/css" :
            extension == "html" ? "text/html" :
            extension == "txt"  ? "text/plain" :
            extension == "jpeg" ? "image/jpeg" :
            extension == "jpg"  ? "image/jpeg" :
            extension == "png"  ? "image/png" :
            extension == "gif"  ? "image/gif" :
            extension == "mp4"  ? "video/mp4" :
            extension == "mpeg" ? "audio/mpeg" :
            extension == "mp3"  ? "audio/mpeg" :
            "application/octet-stream" // Default to binary data
        ;
    }
}

#endif