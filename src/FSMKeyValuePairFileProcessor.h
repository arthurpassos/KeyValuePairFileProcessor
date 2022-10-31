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

    struct NextState {
        std::size_t pos;
        State state;
    };

public:
    FSMKeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character);

    [[nodiscard]] std::map<std::string, std::string> process(const std::string & file) const;

private:
    char item_delimiter;
    char key_value_delimiter;
    char escape_character;
    std::optional<char> enclosing_character;

    FSMKeyValuePairFileProcessor::NextState waitKey(const std::string &file, size_t pos) const;
    FSMKeyValuePairFileProcessor::NextState readKey(const std::string &file, size_t pos, std::string &key) const;
    FSMKeyValuePairFileProcessor::NextState readEnclosedKey(const std::string &file, size_t pos, std::string &key) const;
    NextState readKeyValueDelimiter(const std::string & file, std::size_t pos, State state) const;
    FSMKeyValuePairFileProcessor::NextState waitValue(const std::string &file, size_t pos) const;
    NextState readValue(const std::string & file, std::size_t pos, State state, std::string & value) const;
    NextState readEnclosedValue(const std::string & file, std::size_t pos, State state, std::string & value) const;
    FSMKeyValuePairFileProcessor::NextState
    flushPair(const std::string &file, std::size_t pos, std::string &key, std::string &value,
              std::map<std::string, std::string> &response) const;

    NextState process(
            const std::string & file, std::size_t pos,
            State state, std::string & key, std::string & value, std::map<std::string, std::string> & response) const;

};
