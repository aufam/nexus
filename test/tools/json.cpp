#include "gtest/gtest.h"
#include "nexus/tools/json.h"
#include <etl/keywords.h>

TEST(tools, json) {
    var concat = nexus::tools::json_concat(nexus::tools::jsonify("age", 30), nexus::tools::jsonify("test", 1));
    EXPECT_EQ(concat, "{\"age\": 30, \"test\": 1}");

    var vec = nexus::tools::json_to_vector<uint8_t>(nexus::tools::json_parse(std::string_view("[1, 2, 3]")));
    EXPECT_EQ(vec[0], 1);
    EXPECT_EQ(vec[1], 2);
    EXPECT_EQ(vec[2], 3);
}

