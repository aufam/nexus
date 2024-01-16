#include "nexus/version.h"
#include <iostream>
#include <etl/keywords.h>

int main() {
    std::cout << "Nexus version: " << nexus::get_version() << '\n';
    return 0;
}