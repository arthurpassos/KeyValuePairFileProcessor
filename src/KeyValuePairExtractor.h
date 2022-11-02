#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>

class KeyValuePairExtractor {

    static constexpr char SPACE_CHARACTER = ' ';
    enum State {
        WAITING_KEY,
        READING_KEY,
        READING_ENCLOSED_KEY,
        READING_KV_DELIMITER,
        WAITING_VALUE,
        READING_VALUE,
        READING_ENCLOSED_VALUE,
        FLUSH_PAIR,
        END
    };

    struct NextState {
        std::size_t pos;
        State state;
    };

public:
    KeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] std::map<std::string, std::string> extract(const std::string & file) const;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    KeyValuePairExtractor::NextState waitKey(const std::string &file, size_t pos) const;
    KeyValuePairExtractor::NextState readKey(const std::string &file, size_t pos, std::string &key) const;
    KeyValuePairExtractor::NextState readEnclosedKey(const std::string &file, size_t pos, std::string &key) const;
    KeyValuePairExtractor::NextState readKeyValueDelimiter(const std::string &file, size_t pos) const;
    KeyValuePairExtractor::NextState waitValue(const std::string &file, size_t pos) const;
    KeyValuePairExtractor::NextState readValue(const std::string &file, size_t pos, std::string &value) const;
    KeyValuePairExtractor::NextState
    readEnclosedValue(const std::string &file, size_t pos, std::string &value) const;
    KeyValuePairExtractor::NextState
    flushPair(const std::string &file, std::size_t pos, std::string &key, std::string &value,
              std::map<std::string, std::string> &response) const;

    NextState extract(
            const std::string & file, std::size_t pos,
            State state, std::string & key, std::string & value, std::map<std::string, std::string> & response) const;

};
