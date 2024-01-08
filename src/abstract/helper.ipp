template <typename... Args>
auto static get_restful_response(char* (*override_function)(Args...), Args... args) {
    auto res = override_function ? override_function(args...) : nullptr;
    std::string str = res;
    if (res) ::free(res);
    return str;
}

#include <cstring>

auto static c_string [[maybe_unused]] (const std::string& text) { 
    auto res = (char*) ::malloc(text.size() + 1);
    ::strncpy(res, text.data(), text.size());
    return res;
}