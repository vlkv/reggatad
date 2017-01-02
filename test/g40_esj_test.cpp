#include <gtest/gtest.h>
#include <esj/json_reader.h>
#include <esj/json_writer.h>

//-----------------------------------------------------------------------------
// absolutely minimal example

class JSONExample {
public:
    // content gets streamed to JSON
    std::string text;
public:

    void serialize(JSON::Adapter& adapter) {
        // this pattern is required
        JSON::Class root(adapter, "JSONExample");
        // this is the last member variable we serialize so use the _T variant
        JSON_T(adapter, text);
    }
};

TEST(G40EsjTest, Simple) {
    JSONExample source;
    source.text = "Hello World 2014";

    std::string json = JSON::producer<JSONExample>::convert(source);
    ASSERT_EQ("{\"JSONExample\":{\"text\":\"Hello World 2014\"}}", json);

    JSONExample sink = JSON::consumer<JSONExample>::convert(json);
    ASSERT_EQ(source.text, sink.text);
}