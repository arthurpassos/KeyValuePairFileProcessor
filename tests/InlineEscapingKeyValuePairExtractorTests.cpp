#include <fstream>

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "KeyValuePairExtractorTest.h"
#include "InlineEscapingKeyValuePairExtractor.h"


struct InlineEscapingKeyValuePairExtractorTestCase : KeyValuePairExtractorTestCase {

};

struct InlineEscapingKeyValuePairExtractorTest : public ::testing::TestWithParam<InlineEscapingKeyValuePairExtractorTestCase> {

};;

TEST_P(InlineEscapingKeyValuePairExtractorTest, DefaultSpecialCharacters) {
    const auto & [input, expected_output, item_delimiter, key_value_delimiter, escape_character, enclosing_character] = GetParam();

    InlineEscapingKeyValuePairExtractor processor(item_delimiter, key_value_delimiter, escape_character, enclosing_character);

    auto result = processor.extract(input);

    EXPECT_EQ(result, expected_output);
}

INSTANTIATE_TEST_SUITE_P(
    ValuesCanBeEmptyString,
    InlineEscapingKeyValuePairExtractorTest,
    ::testing::ValuesIn(std::initializer_list<InlineEscapingKeyValuePairExtractorTestCase> {
        {
            "age:",
            {
                {"age", ""}
            },
        },
        {
            "name: neymar, favorite_movie:,favorite_song:",
            {
                {"name", "neymar"},
                {"favorite_movie", ""},
                {"favorite_song", ""},
            }
        }
    })
);

INSTANTIATE_TEST_SUITE_P(
    MixString,
    InlineEscapingKeyValuePairExtractorTest,
    ::testing::ValuesIn(std::initializer_list<InlineEscapingKeyValuePairExtractorTestCase> {
        R"(9 ads =nm,  no\:me: neymar, age: 30, daojmskdpoa and a  height:   175, school: lupe\ picasso, team: psg,)",
        {
            {R"(no:me)", "neymar"},
            {"age", "30"},
            {"height", "175"},
            {"school", "lupe picasso"},
            {"team", "psg"}
        }
    })
);

INSTANTIATE_TEST_SUITE_P(
    Escaping,
    InlineEscapingKeyValuePairExtractorTest,
    ::testing::ValuesIn(std::initializer_list<InlineEscapingKeyValuePairExtractorTestCase> {
        {
            "na,me,: neymar, age:30",
             {
                 {"age", "30"}
             }
        },
        {
            "na$me,: neymar, age:30",
            {
                    {"age", "30"}
            }
        },
        {
            R"(name: neymar, favorite_quote: Premature\ optimization\ is\ the\ r\$\$t\ of\ all\ evil, age:30)",
            {
                {"name", "neymar"},
                {"favorite_quote", R"(Premature optimization is the r$$t of all evil)"},
                {"age", "30"}
            },
            ',',
            ':',
            '\\',
            '"'
        }
    })
);

TEST(InlineEscapingKeyValuePairExtractorTests, MixString2) {

    std::ifstream input_file("/home/arthur/CLionProjects/key_value_pair_processing/tests/kvp_test_file.txt");

    std::ostringstream ss;
    ss << input_file.rdbuf();

    std::string input_string = ss.str();

    using json = nlohmann::json;

    std::ifstream expected_output_file("/home/arthur/CLionProjects/key_value_pair_processing/tests/kvp_expected_output.json");

    json expected_output_json = json::parse(expected_output_file);

    auto expected_output = expected_output_json.get<std::unordered_map<std::string, std::string>>();

    InlineEscapingKeyValuePairExtractor processor(',', '=', '\\', '\"');

    auto result = processor.extract(input_string);

    EXPECT_EQ(result, expected_output);
}

TEST(InlineEscapingKeyValuePairExtractorTests, MixString3) {

    std::ifstream input_file("/home/arthur/CLionProjects/key_value_pair_processing/tests/big_input_file.txt");

    std::ostringstream ss;
    ss << input_file.rdbuf();

    std::string input_string = ss.str();

    using json = nlohmann::json;

    std::ifstream expected_output_file("/home/arthur/CLionProjects/key_value_pair_processing/tests/big_output_file.json");

    json expected_output_json = json::parse(expected_output_file);

    auto expected_output = expected_output_json.get<std::unordered_map<std::string, std::string>>();

    InlineEscapingKeyValuePairExtractor processor(',', ':', '\\', '\"');

    auto result = processor.extract(input_string);

    EXPECT_EQ(result, expected_output);
}
