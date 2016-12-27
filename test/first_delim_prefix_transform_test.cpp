#include <first_delim_prefix_transform.h>
#include <gtest/gtest.h>

TEST(FirstDelimPrefixTransformTest, Simple) {
    auto pe = std::unique_ptr<rocksdb::SliceTransform>(new FirstDelimPrefixTransform());
    ASSERT_EQ("tag1", pe->Transform("tag1:tag2").ToString());
    ASSERT_EQ("tag1", pe->Transform("tag1:tag2:tag3").ToString());
    ASSERT_EQ("tag1\\:tag2", pe->Transform("tag1\\:tag2:tag3").ToString());
    ASSERT_EQ("tag1", pe->Transform("tag1:tag2\\:tag3").ToString());
    ASSERT_EQ("tag1\\tag2", pe->Transform("tag1\\tag2:tag3").ToString());
    ASSERT_EQ("", pe->Transform("").ToString());
    ASSERT_EQ("", pe->Transform(":").ToString());
    ASSERT_EQ("tag", pe->Transform("tag:").ToString());
    ASSERT_EQ("", pe->Transform(":tag").ToString());
    ASSERT_EQ("\\:", pe->Transform("\\:").ToString());
    ASSERT_EQ("tag\\:", pe->Transform("tag\\:").ToString());
    ASSERT_EQ("\\:tag", pe->Transform("\\:tag").ToString());
    ASSERT_EQ("tag", pe->Transform("tag").ToString());
}