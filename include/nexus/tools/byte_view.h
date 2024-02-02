#ifndef PROJECT_NEXUS_TOOLS_BYTE_VIEW_H
#define PROJECT_NEXUS_TOOLS_BYTE_VIEW_H

#include "nexus/tools/check_index.h"
#include <vector>
#include <string>

namespace Project::nexus {
    class byte_view {
    public:
        byte_view() : buf(nullptr), length(0) {}
        byte_view(const uint8_t* buf, size_t length) : buf(buf), length(length) {} 
        
        byte_view(const byte_view& other) : buf(other.buf), length(other.length), vec(std::move(other.vec)) {} 
        byte_view& operator=(const byte_view& other) { buf = other.buf; length = other.length; vec = std::move(other.vec); return *this; }

        byte_view(const std::vector<uint8_t>& v) : byte_view(v.data(), v.size()) {}
        byte_view(std::vector<uint8_t>&& v) : buf(v.data()), length(v.size()), vec(std::move(v)) {}

        byte_view(std::string_view s) : byte_view(reinterpret_cast<const uint8_t*>(s.data()), s.size()) {}
        byte_view(const std::string& s) : byte_view(reinterpret_cast<const uint8_t*>(s.data()), s.size()) {}
        byte_view(std::string&& s) : buf(nullptr), length(0), vec(s.begin(), s.end()) {
            buf = vec.data();
            length = vec.size();
        }

        byte_view(std::initializer_list<uint8_t>&& l) : buf(nullptr), length(0), vec(std::move(l)) {
            buf = vec.data();
            length = vec.size();
        }

        template <size_t N>
        byte_view(const uint8_t(&data)[N]) : byte_view(data, N) {}

        auto data() const { return buf; }
        auto len() const { return length; }
        auto size() const { return length; }
        
        auto begin() const { return buf; }
        auto end() const { return buf + length; }

        auto &front() const { return *buf; }
        auto &back() const { return buf[length - 1]; }
        bool empty() const { return length == 0; }

        byte_view copy() const { return std::vector(buf, buf + length); }
        byte_view move() const { return *this; }

        auto &operator[](int index) const { tools::check_index(*this, index); return buf[index]; }

        bool operator==(byte_view other) const { return std::equal(begin(), end(), other.begin(), other.end()); }
        bool operator!=(byte_view other) const { return !operator==(other); }

        std::string_view to_string() const { return {reinterpret_cast<const char*>(buf), length}; }
        std::vector<uint8_t> to_vector() const { return vec.empty() ? std::vector<uint8_t>{buf, buf + length} : std::move(vec); }

        operator std::vector<uint8_t>() const { return to_vector(); }
        operator std::string_view() const { return to_string(); }
    
    protected:
        const uint8_t* buf;
        size_t length;
        mutable std::vector<uint8_t> vec;
    };
}

#endif