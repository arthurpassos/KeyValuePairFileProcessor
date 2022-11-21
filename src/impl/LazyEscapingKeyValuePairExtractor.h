#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>

#include "KeyValuePairExtractor.h"
#include "State.h"
#include "KeyStateHandler.h"
#include "ValueStateHandler.h"

class LazyEscapingKeyValuePairExtractor : public KeyValuePairExtractor {
public:
    LazyEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] Response extract(const std::string & file) override;

private:
    NextState extract(const std::string & file, std::size_t pos, State state);

    NextState waitKey(const std::string & file, size_t pos) const;
    NextState readKey(const std::string & file, size_t pos);
    NextState readEnclosedKey(const std::string  &file, size_t pos);
    NextState readKeyValueDelimiter(const std::string & file, size_t pos) const;

    NextState waitValue(const std::string & file, size_t pos) const;
    NextState readValue(const std::string & file, size_t pos);
    NextState readEnclosedValue(const std::string & file, size_t pos);
    NextState readEmptyValue(const std::string & file, size_t pos);

    NextState flushPair(const std::string & file, std::size_t pos);

    char escape_character;

    KeyStateHandler keyStateHandler;
    ValueStateHandler valueStateHandler;

    std::string_view key;
    std::string_view value;
    std::unordered_map<std::string_view, std::string_view> response_views;

};
