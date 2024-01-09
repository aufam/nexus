namespace Project::nexus {
    const char* get_version() { return NEXUS_VERSION; }
    int get_version_major() { return NEXUS_VERSION_MAJOR; }
    int get_version_minor() { return NEXUS_VERSION_MINOR; }
    int get_version_patch() { return NEXUS_VERSION_PATCH; }
}

extern "C" {
    const char* nexus_get_version() { return NEXUS_VERSION; }
    int nexus_get_version_major() { return NEXUS_VERSION_MAJOR; }
    int nexus_get_version_minor() { return NEXUS_VERSION_MINOR; }
    int nexus_get_version_patch() { return NEXUS_VERSION_PATCH; }
}
