#ifndef PROJECT_NEXUS_TOOLS_AWAIT_H
#define PROJECT_NEXUS_TOOLS_AWAIT_H

#include <future>

namespace Project::nexus {
    // just syntax sugar
    inline struct Await {        
        template <typename T> 
        T operator|(std::future<T>& future) const { return future.get(); }
    } await;
}

#endif