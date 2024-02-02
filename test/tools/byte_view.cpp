#include "gtest/gtest.h"
#include "nexus/tools/byte_view.h"
#include <etl/keywords.h>

TEST(tools, byte_view) {
    var a = std::vector<uint8_t>({1, 2, 3});
    var bv = nexus::byte_view(a);   // bv only holds the pointer
    EXPECT_EQ(a.data(), bv.data());
    EXPECT_EQ(a.size(), bv.size());

    bv = std::move(a); // bv holds the vector as well
    var b = std::vector<uint8_t>(bv); // bv is moved to b, bv only holds the pointer 
    EXPECT_EQ(b.data(), bv.data());
    EXPECT_EQ(b.size(), bv.size());
    EXPECT_TRUE(a.empty());

    var c = std::move(b);
    EXPECT_EQ(c.data(), bv.data());
    EXPECT_EQ(c.size(), bv.size());
    EXPECT_TRUE(b.empty());

    bv = std::move(c);
    EXPECT_TRUE(c.empty());

    // backward indexing
    EXPECT_EQ(bv[-1], 3);
    EXPECT_EQ(bv[-2], 2);
    EXPECT_EQ(bv[-3], 1);
}
