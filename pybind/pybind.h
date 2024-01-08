#ifndef PROJECT_NEXUS_PYBIND_H
#define PROJECT_NEXUS_PYBIND_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <termios.h>
#include "nexus/tools/byte_view.h"

namespace pybind11 { 

    class BaudRate {
        int value;
    public:
        BaudRate(int value) : value(value) {}
        operator int() const { return value; }
    };

    class byte_view : public Project::nexus::byte_view {
    public:
        byte_view(Project::nexus::byte_view bv) : Project::nexus::byte_view(nullptr, 0), v(bv.begin(), bv.end()) {
            buf = v.data();
            length = v.size();
        }

        byte_view(std::vector<uint8_t>&& bv) : Project::nexus::byte_view(nullptr, 0), v(std::move(bv)) {
            buf = v.data();
            length = v.size();
        }

        byte_view(std::string_view sv) : Project::nexus::byte_view(nullptr, 0), v(sv.begin(), sv.end()) {
            buf = v.data();
            length = v.size();
        }

        byte_view(const object &o) : Project::nexus::byte_view(nullptr, 0) {
            gil_scoped_acquire gil_acquire;
            if (isinstance<str>(o)) {
                auto s = o.cast<std::string>();
                v = std::vector<uint8_t>(s.begin(), s.end());
            } else if (isinstance<list>(o)) {
                auto l = o.cast<list>();
                v.reserve(l.size());
                for (auto &item : l)
                    v.push_back(item.cast<uint8_t>());
            } else if (isinstance<bytes>(o)) {
                auto b = o.cast<bytes>();
                v.reserve(pybind11::len(b));
                for (auto &item : b)
                    v.push_back(item.cast<uint8_t>());
            } else if (isinstance<memoryview>(o)) {
                auto b = o.cast<memoryview>();
                v.reserve(pybind11::len(b));
                for (auto &item : b)
                    v.push_back(item.cast<uint8_t>());
            } else {
                std::invalid_argument("Unsupported object type");
            }

            buf = v.data();
            length = v.size();
        }

        std::vector<uint8_t> v;
    };
}

#endif