#include "FSMKeyValuePairFileProcessor.h"

#include <stdexcept>
#include <optional>

FSMKeyValuePairFileProcessor::FSMKeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
        : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {

}

std::map<std::string, std::string> FSMKeyValuePairFileProcessor::process(const std::string & file) const {

    auto state = State::WAITING_KEY;

    std::map<std::string, std::string> response;

    std::size_t pos = 0;

    std::string key;
    std::string value;

    while (state != State::END) {
        switch (state) {
            case State::WAITING_KEY:
                waitKey(file, pos, state);
                break;
            case State::READING_KEY:
                key = readKey(file, pos, state);
                break;
            case State::READING_ENCLOSED_KEY:
                key = readEnclosedKey(file, pos, state);
                break;
            case State::READING_KV_DELIMITER:
                readKeyValueDelimiter(file, pos, state);
                break;
            case State::WAITING_VALUE:
                waitValue(file, pos, state);
                break;
            case State::READING_VALUE:
                value = readValue(file, pos, state);
                break;
            case State::READING_ENCLOSED_VALUE:
                value = readEnclosedValue(file, pos, state);
                break;
            case State::FLUSH_PAIR:
                response[key] = value;
                if (pos == file.size()) {
                    state = END;
                } else {
                    state = State::WAITING_KEY;
                }
                break;
            case END:
                break;
        }
    }

    return response;
}

void FSMKeyValuePairFileProcessor::waitKey(const std::string & file, size_t & pos, State & state) const {

    if (pos == file.size()) {
        state = END;
        return;
    }

    while (pos < file.size() && state == WAITING_KEY) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER != current_character) {
            state = READING_KEY;
        } else {
            pos++;
        }
    }
}

std::string FSMKeyValuePairFileProcessor::readKey(const std::string & file, size_t & pos, State & state) const {
    std::string key;
    bool escape = false;

    while (pos < file.size() && state == READING_KEY) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            state = READING_ENCLOSED_KEY;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == key_value_delimiter) {
            state = WAITING_VALUE;
        } else if (SPACE_CHARACTER == current_character || current_character == item_delimiter) {
            state = WAITING_KEY;
        } else {
            key.push_back(current_character);
        }
    }

    if (key.empty() && state != READING_ENCLOSED_KEY) {
        state = WAITING_KEY;
    }

    return key;
}

std::string FSMKeyValuePairFileProcessor::readEnclosedKey(const std::string & file, size_t & pos, State & state) const {
    std::string key;
    bool escape = false;

    while (pos < file.size() && state == READING_ENCLOSED_KEY) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character == current_character) {
            state = READING_KV_DELIMITER;
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            key.push_back(current_character);
        }
    }

    if (key.empty()) {
        state = WAITING_KEY;
    }

    return key;
}

void FSMKeyValuePairFileProcessor::readKeyValueDelimiter(const std::string & file, size_t & pos, State & state) const {

    if (pos == file.size()) {
        state = END;
        return;
    }

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
    if (pos == file.size()) {
        state = READING_VALUE;
        return;
    }

    while (pos < file.size() && state == WAITING_VALUE) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER == current_character) {
            pos++;
        } else {
            state = READING_VALUE;
        }
    }
}

std::string FSMKeyValuePairFileProcessor::readValue(const std::string & file, size_t & pos, State & state) const {
    std::string value;
    bool escape = false;

    if (pos == file.size()) {
        state = FLUSH_PAIR;
        return "";
    }

    while (pos < file.size() && state == READING_VALUE) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            state = READING_ENCLOSED_VALUE;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter) {
            state = FLUSH_PAIR;
        } else {
            value.push_back(current_character);
        }
    }

    if (pos == file.size() && state == READING_VALUE) {
        state = FLUSH_PAIR;
    }

    return value;
}

std::string FSMKeyValuePairFileProcessor::readEnclosedValue(const std::string & file, size_t & pos, State & state) const {
    std::string value;
    bool escape = false;

    while (pos < file.size() && state == READING_ENCLOSED_VALUE) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character == current_character) {
            state = FLUSH_PAIR;
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            value.push_back(current_character);
        }
    }

    return value;
}
