#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>

class KeyValuePairExtractor {
protected:
    static constexpr char SPACE_CHARACTER = ' ';
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
    using Response = std::unordered_map<std::string, std::string>;

    KeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] Response extract(const std::string & file);

protected:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    KeyValuePairExtractor::NextState waitKey(const std::string & file, size_t pos) const;
    virtual KeyValuePairExtractor::NextState readKey(const std::string & file, size_t pos) = 0;
    virtual KeyValuePairExtractor::NextState readEnclosedKey(const std::string  &file, size_t pos) = 0;
    KeyValuePairExtractor::NextState readKeyValueDelimiter(const std::string & file, size_t pos) const;
    KeyValuePairExtractor::NextState waitValue(const std::string & file, size_t pos) const;
    virtual KeyValuePairExtractor::NextState readValue(const std::string & file, size_t pos) = 0;
    virtual KeyValuePairExtractor::NextState readEnclosedValue(const std::string & file, size_t pos) = 0;
    virtual KeyValuePairExtractor::NextState readEmptyValue(const std::string & file, size_t pos) = 0;
    virtual KeyValuePairExtractor::NextState flushPair(const std::string & file, std::size_t pos) = 0;

    virtual Response get() const = 0;


    NextState extract(const std::string & file, std::size_t pos, State state);

};
