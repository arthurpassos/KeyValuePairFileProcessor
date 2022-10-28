#include "FSMKeyValuePairFileProcessor.h"

#include <stdexcept>
#include <optional>
#include <vector>
#include <functional>

FSMKeyValuePairFileProcessor::FSMKeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
        : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {

}

std::map<std::string, std::string> FSMKeyValuePairFileProcessor::process(const std::string & file) const {

    auto state = State::WAITING_KEY;

    std::size_t pos = 0;
    std::optional<std::string> key;
    std::optional<std::string> value;

    std::map<std::string, std::string> response;

    while (pos < file.size() || state == State::FLUSH_PAIR) {
        switch (state) {
            case State::WAITING_KEY:
                waitKey(file, pos, state);
                break;
            case State::READING_KEY:
                key = readKey(file, pos, state);
                break;
            case State::READING_KV_DELIMITER:
                readKeyValueDelimiter(file, pos, state);
                break;
            case State::WAITING_VALUE:
                waitValue(file, pos, state);
                break;
            case State::READING_VALUE:
                value = readValue(file, pos, state);
                if (!value) {
                    key = std::nullopt;
                }
                break;
            case State::FLUSH_PAIR:
                response[*key] = *value;
                state = State::WAITING_KEY;
                break;
        }
    }

    return response;
}

void FSMKeyValuePairFileProcessor::waitKey(const std::string & file, size_t & pos, State & state) const {
    while (pos < file.size() && state == WAITING_KEY) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER != current_character) {
            state = READING_KEY;
        } else {
            pos++;
        }
    }
}

std::optional<std::string> FSMKeyValuePairFileProcessor::readKey(const std::string & file, size_t & pos, State & state) const {
    std::vector<char> key;
    bool escape = false;
    bool enclose_start_found = false;

    while (pos < file.size() && state == READING_KEY) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            if (enclose_start_found) {
                state = State::READING_KV_DELIMITER;
                enclose_start_found = false;
            } else {
                enclose_start_found = true;
            }
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == key_value_delimiter && !enclose_start_found) {
            state = WAITING_VALUE;
        } else if (SPACE_CHARACTER == current_character && !enclose_start_found) {
            state = WAITING_KEY;
            key.clear();
        } else {
            key.push_back(current_character);
        }
    }

    return key.empty() ? std::nullopt : std::make_optional<std::string>(key);
}

void FSMKeyValuePairFileProcessor::readKeyValueDelimiter(const std::string & file, size_t & pos, State & state) const {
    while (pos < file.size() && state == READING_KV_DELIMITER) {
        const auto current_character = file[pos++];
        if (current_character != key_value_delimiter) {
            state = WAITING_KEY;
        } else {
            state = WAITING_VALUE;
        }
    }
}

void FSMKeyValuePairFileProcessor::waitValue(const std::string & file, size_t & pos, State & state) const {
    while (pos < file.size() && state == WAITING_VALUE) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER == current_character) {
            pos++;
        } else {
            state = READING_VALUE;
        }

    }
}

std::optional<std::string> FSMKeyValuePairFileProcessor::readValue(const std::string & file, size_t & pos, State & state) const {
    std::vector<char> value;
    bool escape = false;
    bool enclose_start_found = false;

    while (pos < file.size() && state == READING_VALUE) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            if (enclose_start_found) {
                state = State::FLUSH_PAIR;
                enclose_start_found = false;
            } else {
                enclose_start_found = true;
            }
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter && !enclose_start_found) {
            state = FLUSH_PAIR;
        } else {
            value.push_back(current_character);
        }
    }

    return value.empty() ? std::nullopt : std::make_optional<std::string>(value);
}
