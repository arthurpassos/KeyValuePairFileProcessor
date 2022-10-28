#pragma once

#include <map>
#include <string>
#include <optional>

class KeyValuePairFileProcessor {
    static constexpr char SPACE_CHARACTER = ' ';

    enum State {
        WAITING_KEY,
        READING_KEY,
        READING_VALUE,
    };

public:
    KeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] std::map<std::string, std::string> process(const std::string & file) const;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    std::string getKey(const std::string & file, std::size_t & pos) const;
    std::string getValue(const std::string & file, std::size_t & pos) const;

    void skipKeyValueDelimiter(const std::string & file, std::size_t & pos) const;
    void skipItemDelimiter(const std::string & file, std::size_t & pos) const;

    void skipElement(char item, const std::string & file, std::size_t & pos) const;
};

