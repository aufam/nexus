#include "gtest/gtest.h"
#include "nexus/tools/to_string.h"
#include <etl/keywords.h>

TEST(tools, to_string) {
    var a = nexus::tools::to_string(std::string("abc"));
    EXPECT_EQ(a, "\"abc\"");

    var b = nexus::tools::to_string(std::function([] { return true; }));
    EXPECT_EQ(b, "true");

    var c = nexus::tools::to_string(std::vector<int>());
    EXPECT_EQ(c, "[]");

    var d = nexus::tools::to_string(std::vector<std::string_view>({"abc", "def", "1234", }));
    EXPECT_EQ(d, "[\"abc\", \"def\", \"1234\"]");

    var e = nexus::tools::to_string(std::unordered_map<int, int>());
    EXPECT_EQ(e, "{}");

    var f = nexus::tools::to_string(std::unordered_map<std::string, int>({{"one", 1}, {"two", 2}, }));
    EXPECT_EQ(f, "{\"two\": 2, \"one\": 1}");

    var g = nexus::tools::to_string(std::unordered_map<uint8_t, int>({{1, 1}, {2, 2}, }));
    EXPECT_EQ(g, "{\"2\": 2, \"1\": 1}");
}
