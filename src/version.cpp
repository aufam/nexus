#ifndef PROJECT_NEXUS_VERSION_H
#define PROJECT_NEXUS_VERSION_H
#include "nexus/version.h"

#ifdef __cplusplus

namespace Project::nexus {
    const char* get_version();
    int get_version_major();
    int get_version_minor();
    int get_version_patch();
}

extern "C" {
#endif

const char* nexus_get_version();
int nexus_get_version_major();
int nexus_get_version_minor();
int nexus_get_version_patch();

#ifdef __cplusplus
}
#endif
#endif