#pragma once

#include <string_view>
#include "KeyValuePairExtractor.h"

class LazyEscapingKeyValuePairExtractor : public KeyValuePairExtractor {
public:
    LazyEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

private:
    std::string_view key;
    std::string_view value;
    std::unordered_map<std::string_view, std::string_view> response_views;

    KeyValuePairExtractor::NextState readKey(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEnclosedKey(const std::string & file, size_t pos) override;

    KeyValuePairExtractor::NextState readValue(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEnclosedValue(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEmptyValue(const std::string & file, size_t pos) override;

    KeyValuePairExtractor::NextState flushPair(const std::string & file, std::size_t pos) override;

    Response get() const override;

    std::string_view createElement(const std::string & file, std::size_t being, std::size_t end);
};
