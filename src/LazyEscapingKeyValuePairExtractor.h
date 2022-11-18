#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>

#include "KeyValuePairExtractor.h"

class LazyEscapingKeyValuePairExtractor : public KeyValuePairExtractor {
private:
    enum State {
        WAITING_KEY,
        READING_KEY,
        READING_ENCLOSED_KEY,
        READING_KV_DELIMITER,
        WAITING_VALUE,
        READING_VALUE,
        READING_ENCLOSED_VALUE,
        READING_EMPTY_VALUE,
        FLUSH_PAIR,
        END
    };

    struct NextState {
        std::size_t pos;
        State state;
    };

public:
    LazyEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] Response extract(const std::string & file) override;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;
    std::string_view key;
    std::string_view value;
    std::unordered_map<std::string_view, std::string_view> response_views;

    LazyEscapingKeyValuePairExtractor::NextState waitKey(const std::string & file, size_t pos) const;
    LazyEscapingKeyValuePairExtractor::NextState readKey(const std::string & file, size_t pos);
    LazyEscapingKeyValuePairExtractor::NextState readEnclosedKey(const std::string  &file, size_t pos);
    LazyEscapingKeyValuePairExtractor::NextState readKeyValueDelimiter(const std::string & file, size_t pos) const;
    LazyEscapingKeyValuePairExtractor::NextState waitValue(const std::string & file, size_t pos) const;
    LazyEscapingKeyValuePairExtractor::NextState readValue(const std::string & file, size_t pos);
    LazyEscapingKeyValuePairExtractor::NextState readEnclosedValue(const std::string & file, size_t pos);
    LazyEscapingKeyValuePairExtractor::NextState readEmptyValue(const std::string & file, size_t pos);
    LazyEscapingKeyValuePairExtractor::NextState flushPair(const std::string & file, std::size_t pos);

    Response get() const;


    NextState extract(const std::string & file, std::size_t pos, State state);

    std::string_view createElement(const std::string & file, std::size_t being, std::size_t end);

};
