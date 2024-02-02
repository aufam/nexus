#include "nexus/modbus/api.h"
#include <etl/keywords.h>

int main(int argc, char* argv[]) {
    auto buffer = std::vector<uint8_t>(argc - 1);
    for (int i in etl::range(1, argc)) {
        sscanf(argv[i], "%02hhx", &buffer[i - 1]);
    }

    auto encoded = nexus::modbus::api::Codec().encode(buffer);
    for (auto byte in encoded) {
        printf("%02hhx ", byte);
    }

    printf("\n");
    return 0;
}