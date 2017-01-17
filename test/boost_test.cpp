#include <gtest/gtest.h>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

TEST(BoostTest, IsRelative) {
    boost::filesystem::path p("./bar/baz");
    ASSERT_EQ(true, p.is_relative());
    ASSERT_EQ(false, p.is_absolute());
}

TEST(BoostTest, IsAbsolute) {
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

TEST(BoostTest, Relative1) {
    auto p = boost::filesystem::relative("/foo/bar/baz/123", "/foo/bar");
    ASSERT_EQ("baz/123", p.string());
}

TEST(BoostTest, Relative2) {
    auto p = boost::filesystem::relative("/foo/bar", "/foo/bar");
    ASSERT_EQ(".", p.string());
}
