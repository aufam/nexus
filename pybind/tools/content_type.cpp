#include "../pybind.h"
#include "nexus/tools/filesystem.h"
#include <etl/keywords.h>

namespace pybind11 { 
    void bindContentType(module_& m); 
}

void pybind11::bindContentType(module_& m) {
    m.def("getContentType", nexus::tools::content_type, arg("file"));
}