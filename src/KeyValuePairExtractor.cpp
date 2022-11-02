#include "KeyValuePairExtractor.h"

#include <optional>

KeyValuePairExtractor::KeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
        : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {

}

std::map<std::string, std::string> KeyValuePairExtractor::extract(const std::string & file) const {

    auto state = State::WAITING_KEY;

    std::map<std::string, std::string> response;

    std::size_t pos = 0;

    std::string key;
    std::string value;

    while (state != State::END) {
        auto nextState = extract(file, pos, state, key, value, response);

        pos = nextState.pos;
        state = nextState.state;
    }

    return response;
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::extract(const std::string & file, std::size_t pos,
                                                                KeyValuePairExtractor::State state,
                                                                std::string &key, std::string &value,
                                                                std::map<std::string, std::string> &response) const {
    switch (state) {
        case State::WAITING_KEY:
            return waitKey(file, pos);
        case State::READING_KEY:
            key.clear();
            return readKey(file, pos, key);
        case State::READING_ENCLOSED_KEY:
            key.clear();
            return readEnclosedKey(file, pos, key);
        case State::READING_KV_DELIMITER:
            return readKeyValueDelimiter(file, pos);
        case State::WAITING_VALUE:
            return waitValue(file, pos);
        case State::READING_VALUE:
            value.clear();
            return readValue(file, pos, value);
        case State::READING_ENCLOSED_VALUE:
            value.clear();
            return readEnclosedValue(file, pos, value);
        case State::FLUSH_PAIR:
            return flushPair(file, pos, key, value, response);
        case END:
            return {
                pos,
                state
            };
    }
}


KeyValuePairExtractor::NextState KeyValuePairExtractor::waitKey(const std::string & file, size_t pos) const {

    while (pos < file.size()) {
        const auto current_character = file[pos];
        if (isalpha(current_character)) {
            return {
                pos,
                State::READING_KEY
            };
        } else if (enclosing_character && current_character == enclosing_character) {
            return {
                pos + 1u,
                State::READING_ENCLOSED_KEY
            };
        } else {
            pos++;
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::readKey(const std::string &file, size_t pos, std::string &key) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == key_value_delimiter) {
            return {
                pos,
                State::WAITING_VALUE
            };
        } else if (std::isalnum(current_character) || current_character == '_') {
            key.push_back(current_character);
        } else {
            return {
                pos,
                State::WAITING_KEY
            };
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::readEnclosedKey(const std::string &file, size_t pos, std::string &key) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character == current_character) {
            return {
                pos,
                State::READING_KV_DELIMITER
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            key.push_back(current_character);
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::readKeyValueDelimiter(const std::string &file, size_t pos) const {

    if (pos == file.size()) {
        return {
            pos,
            State::END
        };
    } else {
        const auto current_character = file[pos++];
        return {
            pos,
            current_character == key_value_delimiter ? State::WAITING_VALUE : State::WAITING_KEY
        };
    }
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::waitValue(const std::string &file, size_t pos) const {

    while (pos < file.size()) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER == current_character) {
            pos++;
        } else {
            break;
        }
    }

    return {
        pos,
        State::READING_VALUE
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::readValue(const std::string &file, size_t pos, std::string &value) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            return {
                pos,
                State::READING_ENCLOSED_VALUE
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter) {
            return {
                pos,
                State::FLUSH_PAIR
            };
        } else {
            value.push_back(current_character);
        }
    }

    // this allows empty values at the end
    return {
        pos,
        State::FLUSH_PAIR
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::readEnclosedValue(const std::string &file, size_t pos, std::string &value) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character == current_character) {
            return {
                pos,
                State::FLUSH_PAIR
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            value.push_back(current_character);
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::flushPair(
        const std::string &file,
        std::size_t pos,
        std::string &key,
        std::string &value,
        std::map<std::string, std::string> &response) const {
    response[std::move(key)] = std::move(value);

    key.clear();
    value.clear();

    return {
        pos,
        pos == file.size() ? State::END : State::WAITING_KEY
    };
}
