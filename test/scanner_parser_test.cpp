#include <parser/parser.h>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

TEST(ScannerParserTest, Simple) {
    std::istringstream is("2 abc");
    std::ostringstream os;
    Parser parser(is, os);
    parser.parse();
    ASSERT_EQ("", os.str());
}
