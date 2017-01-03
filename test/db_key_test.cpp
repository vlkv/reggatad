#include <db_key.h>
#include <gtest/gtest.h>

TEST(DBKeyTest, Join) {
    auto k = DBKey::join("abc:def", "123:456");
    ASSERT_EQ("abc\\:def:123\\:456", k);
}

TEST(DBKeyTest, Join1) {
    auto k = DBKey::join("ab:cd\\ef", "12\\34:56");
    ASSERT_EQ("ab\\:cd\\\\ef:12\\\\34\\:56", k);
}

TEST(DBKeyTest, Join2) {
    auto k = DBKey::join("prefix", "suffix");
    ASSERT_EQ("prefix:suffix", k);
}

TEST(DBKeyTest, Join3) {
    auto k = DBKey::join("", "suffix");
    ASSERT_EQ(":suffix", k);
}

TEST(DBKeyTest, Join4) {
    auto k = DBKey::join("prefix", "");
    ASSERT_EQ("prefix:", k);
}

TEST(DBKeyTest, Join5) {
    auto k = DBKey::join("", "");
    ASSERT_EQ(":", k);
}

TEST(DBKeyTest, Split) {
    auto p = DBKey::split("abc\\:def:123:456\\:789");
    ASSERT_EQ(p.first, "abc\\:def");
    ASSERT_EQ(p.second, "123:456\\:789");
}

TEST(DBKeyTest, Split1) {
    auto p = DBKey::split("prefix:suffix");
    ASSERT_EQ(p.first, "prefix");
    ASSERT_EQ(p.second, "suffix");
}

TEST(DBKeyTest, Split2) {
    auto p = DBKey::split("prefix:");
    ASSERT_EQ(p.first, "prefix");
    ASSERT_EQ(p.second, "");
}

TEST(DBKeyTest, Split3) {
    auto p = DBKey::split(":suffix");
    ASSERT_EQ(p.first, "");
    ASSERT_EQ(p.second, "suffix");
}

TEST(DBKeyTest, Split4) {
    auto p = DBKey::split(":");
    ASSERT_EQ(p.first, "");
    ASSERT_EQ(p.second, "");
}

TEST(DBKeyTest, Split5) {
    auto p = DBKey::split("");
    ASSERT_EQ(p.first, "");
    ASSERT_EQ(p.second, "");
}

TEST(DBKeyTest, Split6) {
    auto p = DBKey::split("prefixsuffix");
    ASSERT_EQ(p.first, "prefixsuffix");
    ASSERT_EQ(p.second, "");
}
