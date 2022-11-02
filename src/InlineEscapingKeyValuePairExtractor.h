#pragma once

#include <string>
#include "KeyValuePairExtractor.h"

class InlineEscapingKeyValuePairExtractor : public KeyValuePairExtractor {

public:
    InlineEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

private:
    std::string key;
    std::string value;
    Response response;

    KeyValuePairExtractor::NextState readKey(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEnclosedKey(const std::string & file, size_t pos) override;

    KeyValuePairExtractor::NextState readValue(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEnclosedValue(const std::string & file, size_t pos) override;
    KeyValuePairExtractor::NextState readEmptyValue(const std::string & file, size_t pos) override;

    KeyValuePairExtractor::NextState flushPair(const std::string & file, std::size_t pos) override;

    Response get() const override;
};
