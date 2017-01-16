#include <gtest/gtest.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

TEST(BoostTest, Relative) {
    boost::filesystem::path p("./bar/baz");
    ASSERT_EQ(true, p.is_relative());
    ASSERT_EQ(false, p.is_absolute());
}

TEST(BoostTest, Absolute) {
    boost::filesystem::path p("/bar/baz");
    ASSERT_EQ(false, p.is_relative());
    ASSERT_EQ(true, p.is_absolute());
}

TEST(BoostTest, Canonical) {
    boost::system::error_code ec;
    auto p = boost::filesystem::canonical("./test_data/non_existent_dir", ec);
    ASSERT_EQ("No such file or directory", ec.message());
    ASSERT_EQ("", p.string());
}