#ifndef PROJECT_NEXUS_TOOLS_CHECK_INDEX_H
#define PROJECT_NEXUS_TOOLS_CHECK_INDEX_H

#include <stdexcept>

namespace Project::nexus::tools {
    template <typename T>
    void check_index(const T& container, int &index) {
        if (index < 0) 
            index += container.size(); // allowing negative index
        
        if (index < 0 || index >= (int) container.size()) 
            throw std::out_of_range("Index is out of range");
    }
}

#endif