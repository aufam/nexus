#include "gtest/gtest.h"
#include "nexus/tools/byte_view.h"
#include <etl/keywords.h>

static nexus::byte_view foo() {
    std::vector<uint8_t> p;
    p.push_back(1);
    p.push_back(2);
    p.push_back(3);
    p.push_back(4);
    p.push_back(5);
    return p;
}

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

    // slice
    bv = nexus::byte_view({1, 2, 3, 4, 5});
    EXPECT_EQ(bv.slice(2), nexus::byte_view({3, 4, 5}));
    EXPECT_EQ(bv.slice(2, 4), nexus::byte_view({3, 4}));
    EXPECT_EQ(bv.slice(0, 5, 2), nexus::byte_view({1, 3, 5}));
    EXPECT_EQ(bv.slice(4, -1, -1), nexus::byte_view({5, 4, 3, 2, 1}));
    EXPECT_EQ(bv, foo());
}
