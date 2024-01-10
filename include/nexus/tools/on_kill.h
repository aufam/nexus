#ifndef PROJECT_NEXUS_TOOLS_ON_KILL_H
#define PROJECT_NEXUS_TOOLS_ON_KILL_H

#include <csignal>
#include <functional>

namespace Project::nexus::tools {
    static std::function<void()> kill_;

    static void on_kill(std::function<void()>&& kill_function) {
        kill_ = std::move(kill_function);
        
        static auto kill_ptr = [] (int signal) {
            if (signal == SIGINT || signal == SIGTERM) {
                kill_();
            }
        };
        
        std::signal(SIGINT, kill_ptr);
        std::signal(SIGTERM, kill_ptr);
    } 
}

#endif