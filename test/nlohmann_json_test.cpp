#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

TEST(NlohmannJsonTest, Simple) {
    nlohmann::json obj{
        {"foo", 1},
        {"subObj",
            {
                {"foo", 2},
                {"foo", 3}
            }}};
    ASSERT_EQ(1, obj.count("foo"));
    ASSERT_EQ(1, obj["subObj"].count("foo"));
    ASSERT_EQ(2, obj["subObj"]["foo"]);
}
