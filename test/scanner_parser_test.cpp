#include <parser/parser.ih>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(ScannerParserTest, Simple) {
    std::istringstream is("(Tag1 & Tag2) | !Tag3");
    std::ostringstream os;
    Parser parser(is, os);
    parser.parse();
    ASSERT_EQ("", os.str());
}
