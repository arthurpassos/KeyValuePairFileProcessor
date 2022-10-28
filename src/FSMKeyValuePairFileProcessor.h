#pragma once

#include <map>
#include <string>
#include <optional>

class FSMKeyValuePairFileProcessor {
    static constexpr char SPACE_CHARACTER = ' ';
    enum State {
        WAITING_KEY,
        READING_KEY,
        READING_KV_DELIMITER,
        WAITING_VALUE,
        READING_VALUE,
        FLUSH_PAIR,
    };

public:
    FSMKeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] std::map<std::string, std::string> process(const std::string & file) const;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    void waitKey(const std::string & file, std::size_t & pos, State & state) const;
    std::optional<std::string> readKey(const std::string & file, std::size_t & pos, State & state) const;
    void readKeyValueDelimiter(const std::string & file, std::size_t & pos, State & state) const;
    void waitValue(const std::string & file, std::size_t & pos, State & state) const;
    std::optional<std::string> readValue(const std::string & file, std::size_t & pos, State & state) const;

};
