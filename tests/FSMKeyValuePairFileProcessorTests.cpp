#include <gtest/gtest.h>
#include "FSMKeyValuePairFileProcessor.h"

TEST(FSMKeyValuePairFileProcessorTests, MixString) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output[R"(no:me)"] = "arthur";
    expected_output["idade"] = "24";
    expected_output["altura"] = "179";
    expected_output["escola"] = "emeb educar";
    expected_output["universidade"] = "univali";

    auto result = processor.process(R"(9 ads =nm,  no\:me: arthur, idade: 24, daojmskdpoa and a  altura:   179, escola: emeb educar, universidade: univali,)");

    EXPECT_EQ(result, expected_output);
}


TEST(FSMKeyValuePairFileProcessorTests, ValuesCanBeEmptyStrings) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output["idade"] = "";
    auto result = processor.process("idade:");

    EXPECT_EQ(result, expected_output);
}

TEST(KeyValuePairFileProcessorTests, DoNotAllowUnescapedSpecialCharactersOnKey) {
    FSMKeyValuePairFileProcessor processor(',', ':', '\\', std::nullopt);

    std::map<std::string, std::string> expected_output;

    expected_output["idade"] = "24";


    auto result = processor.process("no,me,: arthur, idade:24");

    EXPECT_EQ(result, expected_output);
}
