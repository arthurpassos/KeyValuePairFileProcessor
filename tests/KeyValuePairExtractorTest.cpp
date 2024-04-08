#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <KeyValuePairExtractorBuilder.h>


struct LazyKeyValuePairExtractorTestCase {
    std::string input;
    std::unordered_map<std::string, std::string> expected_output;
    std::shared_ptr<KeyValuePairExtractor> extractor;
};

std::ostream & operator<<(std::ostream & ostr, const LazyKeyValuePairExtractorTestCase & test_case)
{
    return ostr << test_case.input;
}

struct KeyValuePairExtractorTest : public ::testing::TestWithParam<LazyKeyValuePairExtractorTestCase> {

};

TEST_P(KeyValuePairExtractorTest, KeyValuePairExtractorTests) {
    const auto & [input, expected_output, extractor] = GetParam();

    auto result = extractor->extract(input);

    EXPECT_EQ(result, expected_output);
}

INSTANTIATE_TEST_SUITE_P(
        CompleteSet,
        KeyValuePairExtractorTest,
        ::testing::Values(
                // Basic tests
                LazyKeyValuePairExtractorTestCase{
                        "name:neymar, age:31 team:psg,nationality:brazil",
                        {{"age", "31"}, {"name", "neymar"}, {"nationality", "brazil"}, {"team", "psg"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Keys and values starting with numbers, underscore, and other special characters
                LazyKeyValuePairExtractorTestCase{
                        "1name:neymar, 4ge:31 _team:_psg,$nationality:@brazil",
                        {{"$nationality", "@brazil"}, {"1name", "neymar"}, {"4ge", "31"}, {"_team", "_psg"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Only special characters
                LazyKeyValuePairExtractorTestCase{
                        "_:_, @:@ #:#,$:$",
                        {{"_", "_"}, {"@", "@"}, {"#", "#"}, {"$", "$"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Special (not control) characters in the middle of elements
                LazyKeyValuePairExtractorTestCase{
                        "name:ney!mar, age:3! t&am:@psg,nationality:br4z!l",
                        {{"age", "3!"}, {"name", "ney!mar"}, {"nationality", "br4z!l"}, {"t&am", "@psg"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Non-standard escape characters, back-slash should be preserved
                LazyKeyValuePairExtractorTestCase{
                        "currency:\\$USD, amount\\z:$5\\h",
                        {{"amount\\z", "$5\\h"}, {"currency", "\\$USD"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Invalid escape sequence at the end of file should be ignored
                LazyKeyValuePairExtractorTestCase{
                        "valid_key:valid_value key:invalid_escape_sequence\\",
                        {{"key", "invalid_escape_sequence"}, {"valid_key", "valid_value"}},
                        KeyValuePairExtractorBuilder().withEscaping().build()
                },
                // Simple quoting
                LazyKeyValuePairExtractorTestCase{
                        "name:\"neymar\", \"age\":31 \"team\":\"psg\"",
                        {{"age", "31"}, {"name", "neymar"}, {"team", "psg"}},
                        KeyValuePairExtractorBuilder().withQuotingCharacter('"').build()
                },
                // Semi-colon as pair delimiter
                LazyKeyValuePairExtractorTestCase{
                        "name:neymar;age:31;team:psg;nationality:brazil",
                        {{"age", "31"}, {"name", "neymar"}, {"nationality", "brazil"}, {"team", "psg"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Both comma and semi-colon as pair delimiters
                LazyKeyValuePairExtractorTestCase{
                        "name:neymar;age:31;team:psg;nationality:brazil,last_key:last_value",
                        {{"age", "31"}, {"last_key", "last_value"}, {"name", "neymar"}, {"nationality", "brazil"}, {"team", "psg"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // Single quote as quoting character
                LazyKeyValuePairExtractorTestCase{
                        "name:'neymar';'age':31;team:psg;nationality:brazil,last_key:last_value",
                        {{"age", "31"}, {"last_key", "last_value"}, {"name", "neymar"}, {"nationality", "brazil"}, {"team", "psg"}},
                        KeyValuePairExtractorBuilder().withQuotingCharacter('\'').build()
                },
                // Formula example with '=' in value
                LazyKeyValuePairExtractorTestCase{
                        "formula=1+2=3 argument1=1 argument2=2 result=3, char=\"=\" char2== string=\"foo=bar\"",
                        {{"argument1", "1"}, {"argument2", "2"}, {"char", "="}, {"char2", "="}, {"formula", "1+2=3"}, {"result", "3"}, {"string", "foo=bar"}},
                        KeyValuePairExtractorBuilder().withKeyValueDelimiter('=').build()
                },
                // non-standard escape characters (i.e not \n, \r, \t and etc), it should accept everything
                LazyKeyValuePairExtractorTestCase{
                        R"(currency:\$USD, amount\z:$5\h)",
                        {{R"(amount\z)", R"($5\h)"}, {"currency", R"(\$USD)"}},
                        KeyValuePairExtractorBuilder().withItemDelimiters({',', ' '}).build()
                },
                // standard escape sequences, it should return it as it is
                LazyKeyValuePairExtractorTestCase{
                        "key1:header\nbody key2:start_of_text\tend_of_text",
                        {{"key1", "header\nbody"}, {"key2", "start_of_text\tend_of_text"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // both comma and semi-colon as pair delimiters
                LazyKeyValuePairExtractorTestCase{
                        "name:neymar;age:31;team:psg;nationality:brazil,last_key:last_value",
                        {{"name", "neymar"}, {"age", "31"}, {"last_key", "last_value"}, {"team", "psg"}, {"nationality", "brazil"}},
                        KeyValuePairExtractorBuilder().build()
                },
                // single quote as quoting character
                LazyKeyValuePairExtractorTestCase{
                        "name:'neymar';'age':31;team:psg;nationality:brazil,last_key:last_value",
                        {{"name", "neymar"}, {"age", "31"}, {"last_key", "last_value"}, {"team", "psg"}, {"nationality", "brazil"}},
                        KeyValuePairExtractorBuilder().withQuotingCharacter('\'').build()
                },
                // should not fail because pair delimiters contains 8 characters, which is within the limit
                LazyKeyValuePairExtractorTestCase{
                        "not_important",
                        {},
                        KeyValuePairExtractorBuilder().withItemDelimiters({'1', '2', '3', '4', '5', '6', '7', '8'}).build()
                }
        )
);

TEST(KeyValuePairExtractorTests, MixString2) {

    std::ifstream input_file("/home/arthur/CLionProjects/KeyValuePairExtractor/tests/kvp_test_file.txt");

    std::ostringstream ss;
    ss << input_file.rdbuf();

    std::string input_string = ss.str();

    using json = nlohmann::json;

    std::ifstream expected_output_file("/home/arthur/CLionProjects/KeyValuePairExtractor/tests/kvp_expected_output.json");

    json expected_output_json = json::parse(expected_output_file);

    auto expected_output = expected_output_json.get<std::unordered_map<std::string, std::string>>();

    auto processor = KeyValuePairExtractorBuilder().withKeyValueDelimiter('=').build();

    auto result = processor->extract(input_string);

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairExtractorTests, MixString3) {

    std::ifstream input_file("/home/arthur/CLionProjects/KeyValuePairExtractor/tests/big_input_file.txt");

    std::ostringstream ss;
    ss << input_file.rdbuf();

    std::string input_string = ss.str();

    using json = nlohmann::json;

    std::ifstream expected_output_file("/home/arthur/CLionProjects/KeyValuePairExtractor/tests/big_output_file.json");

    json expected_output_json = json::parse(expected_output_file);

    auto expected_output = expected_output_json.get<std::unordered_map<std::string, std::string>>();

    auto processor = KeyValuePairExtractorBuilder().withQuotingCharacter('"').build();

    auto result = processor->extract(input_string);

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairExtractorTests, Simplest) {
    auto processor = KeyValuePairExtractorBuilder().build();

    auto result = processor->extract("name:arthur");

    std::unordered_map<std::string, std::string> expected_output { {"name", "arthur"}};

    EXPECT_EQ(result, expected_output);
}

