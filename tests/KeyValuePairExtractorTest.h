#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include <gtest/gtest.h>

struct KeyValuePairExtractorTestCase {
    std::string input;
    std::unordered_map<std::string, std::string> expected_output;
    char item_delimiter = ',';
    char key_value_delimiter = ':';
    char escape_character = '\\';
    std::optional<char> enclosing_character;
};

std::ostream & operator<<(std::ostream & ostr, const KeyValuePairExtractorTestCase & test_case);
