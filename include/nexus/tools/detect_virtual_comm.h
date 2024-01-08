#ifndef PROJECT_NEXUS_TOOLS_DETECT_VIRTUAL_COMM_H
#define PROJECT_NEXUS_TOOLS_DETECT_VIRTUAL_COMM_H

#include <string>
#include <cstring>
#include <vector>
#include <dirent.h>

namespace Project::nexus::tools {

    inline std::vector<std::string> detectVirtualComm() {
        struct dirent *ent;
        std::vector<std::string> res;
        auto dir = opendir("/dev/");
        
        if (dir) {
            while ((ent = readdir(dir)) != nullptr) {
                if (::strstr(ent->d_name, "ttyACM") != nullptr || ::strstr(ent->d_name, "ttyUSB") != nullptr) 
                    res.push_back(std::string("/dev/") + ent->d_name);
            }
        }

        closedir(dir);
        return res;
    }
}

#endif
