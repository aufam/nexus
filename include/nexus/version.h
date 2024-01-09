#ifndef PROJECT_NEXUS_VERSION_H
#define PROJECT_NEXUS_VERSION_H

#ifdef __cplusplus

namespace Project::nexus {
    const char* get_version();
    int get_version_major();
    int get_version_minor();
    int get_version_patch();
}

#else

const char* nexus_get_version();
int nexus_get_version_major();
int nexus_get_version_minor();
int nexus_get_version_patch();

#endif
#endif