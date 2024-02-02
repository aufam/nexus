#include <gtest/gtest.h>
#include "nexus/modbus/api.h"
#include <etl/keywords.h>

static unsigned int crc(unsigned char * data, unsigned char length) {
    unsigned int value = 0xFFFF;
    while(length--) {
        value ^= *data++;
        for(int i = 0; i < 8; i++) {
            if (value & 0x0001) {
                value = (value >> 1) ^ 0xa001;
            } else {
                value = value >> 1;
            }
        }
    }
    return value;
}

TEST(modbus, crc) {
    {
        uint8_t test[] = {0x01, 0x03, 0x80, 0x01, 0x00, 0x01};
        auto mbus_crc1 = nexus::modbus::api::crc(test);
        auto mbus_crc2 = crc(test, sizeof(test));
        EXPECT_EQ(mbus_crc1, mbus_crc2);
    } {
        uint8_t test[] = {0xf8, 0x04, 0x00, 0x00, 0x00, 0x01};
        auto mbus_crc1 = nexus::modbus::api::crc(test);
        auto mbus_crc2 = crc(test, sizeof(test));
        EXPECT_EQ(mbus_crc1, mbus_crc2);
    } {
        uint8_t test[] = {0xf8, 0x04, 0x00, 0x00, 0x00, 0x0a};
        auto mbus_crc1 = nexus::modbus::api::crc(test);
        auto mbus_crc2 = crc(test, sizeof(test));
        EXPECT_EQ(mbus_crc1, mbus_crc2);
    } {
        uint8_t test[] = {0xf8, 0x04, 0x14, 0x08, 0xae, 0x28, 0xc3, 0x00, 0x00, 0x59, 0x6e, 0x00, 0x00, 0x93, 0x6f, 0x00, 0x21, 0x01, 0xf4, 0x00, 0x63, 0x00, 0x00};
        auto mbus_crc1 = nexus::modbus::api::crc(test);
        auto mbus_crc2 = crc(test, sizeof(test));
        EXPECT_EQ(mbus_crc1, mbus_crc2);
    }

}
