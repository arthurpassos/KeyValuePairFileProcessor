#include <gtest/gtest.h>
#include "KeyValuePairFileProcessor.h"

TEST(KeyValuePairFileProcessorTests, MixString) {
    KeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(no:me)"] = "arthur";
    expected_output["idade"] = "24";
    expected_output["altura"] = "179";
    expected_output["escola"] = "emeb educar";
    expected_output["universidade"] = "univali";
    expected_output["music"] = "";

    auto result = processor.process(R"(9 ads =nm,  no\:me: arthur, idade: 24, daojmskdpoa and a  altura:   179, escola: emeb educar, universidade: univali, music:,)");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, EmptyKeyNotAllowed) {
    KeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(nome)"] = "arthur";
    expected_output["idade"] = "24";

    auto result = processor.process(R"(nome: arthur, idade: 24,: univali, music:,)");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, DoNotAllowUnescapedSpecialCharactersOnKey) {
    KeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output["nome"] = "arthur";
    expected_output["idade"] = "24";

    auto result = processor.process(R"(nome: arthur, idade: 24,:: univali, music:,)");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, ValuesCanBeEmptyStrings) {
    KeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(nome)"] = "";
    expected_output["idade"] = "24";

    auto result = processor.process(R"(nome:, idade: 24)");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, EscapeCharOnKey) {
    KeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(no\:me)"] = "arthur";
    expected_output["idade"] = "24";

    auto result = processor.process(R"(no\\\:me: arthur, idade: 24)");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, DoubleQuotesEnclosing) {
    KeyValuePairFileProcessor processor(',', ':', '\\', '"');

    std::map<std::string, std::string> expected_output;

    expected_output[R"(no\:me)"] = "arthur";
    expected_output["idade"] = "24";

    auto result = processor.process(R"("no\\\:me": arthur, "idade": 24)");

    EXPECT_EQ(result, expected_output);
}
