#include <parser/parser.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(ScannerParserTest, Query) {
    std::istringstream is("(Tag1 & Tag2) | !Tag3 Tag4");
    std::ostringstream os;
    Parser parser(is, os);
    parser.parse();
    auto tree = parser.getTree();
    ASSERT_EQ("(AND (OR (AND Tag1 Tag2) (NOT Tag3)) Tag4)", tree->str());
}

TEST(ScannerParserTest, QueryWithSyntaxError) {
    std::istringstream is("(Tag1 & Tag2(");
    std::ostringstream os;
    Parser parser(is, os);
    parser.parse();
    auto tree = parser.getTree();
    ASSERT_EQ(0, tree.use_count());

    auto errors = parser.getErrors();
    ASSERT_EQ(1, errors.size());
    ASSERT_EQ("Syntax error", errors.front());

}
