#pragma once

#include <map>
#include <string>
#include <optional>
#include <functional>

class FSMKeyValuePairFileProcessor {

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

public:
    FSMKeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] std::map<std::string, std::string> process(const std::string & file) const;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    void waitKey(const std::string & file, std::size_t & pos, State & state) const;
    std::string readKey(const std::string & file, std::size_t & pos, State & state) const;
    std::string readEnclosedKey(const std::string & file, std::size_t & pos, State & state) const;
    void readKeyValueDelimiter(const std::string & file, std::size_t & pos, State & state) const;
    void waitValue(const std::string & file, std::size_t & pos, State & state) const;
    std::string readValue(const std::string & file, std::size_t & pos, State & state) const;
    std::string readEnclosedValue(const std::string & file, std::size_t & pos, State & state) const;

};
