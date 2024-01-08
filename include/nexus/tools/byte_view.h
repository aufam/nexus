#ifndef PROJECT_NEXUS_TOOLS_BYTE_VIEW_H
#define PROJECT_NEXUS_TOOLS_BYTE_VIEW_H

#include "nexus/tools/check_index.h"
#include <vector>
#include <string>

namespace Project::nexus {
    class byte_view {
    public:
        byte_view(const uint8_t* buf, size_t length) : buf(buf), length(length) {} 

        byte_view(const std::vector<uint8_t>& v) : byte_view(v.data(), v.size()) {}
        byte_view(std::string_view s) : byte_view(reinterpret_cast<const uint8_t*>(s.data()), s.size()) {}

        template <size_t N>
        byte_view(const uint8_t(&data)[N]) : byte_view(data, N) {}

        template <size_t N>
        byte_view(const char(&data)[N]) : byte_view(reinterpret_cast<const uint8_t*>(data), N) {}

        auto data() const { return buf; }
        auto len() const { return length; }
        auto size() const { return length; }
        
        auto begin() const { return buf; }
        auto end() const { return buf + length; }

        auto &front() const { return *buf; }
        auto &back() const { return buf[length - 1]; }

        auto &operator[](int index) const { tools::check_index(*this, index); return buf[index]; }

        bool operator==(byte_view other) const { return std::equal(begin(), end(), other.begin()); }
        bool operator!=(byte_view other) const { return !operator==(other); }

        operator std::vector<uint8_t>() const { return {buf, buf + length}; }
        operator std::string() const { return {reinterpret_cast<const char*>(buf), reinterpret_cast<const char*>(buf + length)}; }
        operator std::string_view() const { return {reinterpret_cast<const char*>(buf), length}; }
    
    protected:
        const uint8_t* buf;
        size_t length;
    };
}

#endif