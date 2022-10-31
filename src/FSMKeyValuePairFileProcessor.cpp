#include "FSMKeyValuePairFileProcessor.h"

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
        auto nextState = process(file, pos, state, key, value, response);

        pos = nextState.pos;
        state = nextState.state;
    }

    return response;
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::process(const std::string & file, std::size_t pos,
                                                                              FSMKeyValuePairFileProcessor::State state,
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
            return readKeyValueDelimiter(file, pos, state);
        case State::WAITING_VALUE:
            return waitValue(file, pos);
        case State::READING_VALUE:
            value.clear();
            return readValue(file, pos, state, value);
        case State::READING_ENCLOSED_VALUE:
            value.clear();
            return readEnclosedValue(file, pos, state, value);
        case State::FLUSH_PAIR:
            return flushPair(file, pos, key, value, response);
        case END:
            return NextState {
                pos,
                state
            };
    }
}


FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::waitKey(const std::string & file, size_t pos) const {

    while (pos < file.size()) {
        const auto current_character = file[pos];
        if (isalpha(current_character) || enclosing_character && current_character == enclosing_character) {
            return {
                pos,
                State::READING_KEY
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

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::readKey(const std::string &file, size_t pos, std::string &key) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            return {
                pos,
                State::READING_ENCLOSED_KEY
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == key_value_delimiter) {
            return {
                pos,
                State::WAITING_VALUE
            };
        } else if (SPACE_CHARACTER == current_character || current_character == item_delimiter) {
            return {
                pos,
                State::WAITING_KEY
            };
        } else {
            key.push_back(current_character);
        }
    }

    return {
        pos,
        State::END
    };
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::readEnclosedKey(const std::string &file, size_t pos, std::string &key) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            key.push_back(current_character);
        } else if (enclosing_character == current_character) {
            return NextState {
                pos,
                State::READING_KV_DELIMITER
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            key.push_back(current_character);
        }
    }

    return NextState {
        pos,
        State::END
    };
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::readKeyValueDelimiter(const std::string & file, size_t pos, State state) const {

    if (pos == file.size()) {
        return NextState {
            pos,
            State::END
        };
    } else {
        const auto current_character = file[pos++];
        return NextState {
            pos,
            current_character == key_value_delimiter ? State::WAITING_VALUE : State::WAITING_KEY
        };
    }
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::waitValue(const std::string &file, size_t pos) const {

    while (pos < file.size()) {
        const auto current_character = file[pos];
        if (SPACE_CHARACTER == current_character) {
            pos++;
        } else {
            break;
        }
    }

    return NextState {
        pos,
        State::READING_VALUE
    };
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::readValue(const std::string & file, size_t pos, State state, std::string & value) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character && enclosing_character == current_character) {
            return NextState {
                pos,
                State::READING_ENCLOSED_VALUE
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter) {
            return NextState {
                pos,
                State::FLUSH_PAIR
            };
        } else {
            value.push_back(current_character);
        }
    }

    // this allows empty values at the end
    return NextState {
        pos,
        State::FLUSH_PAIR
    };
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::readEnclosedValue(const std::string & file, size_t pos, State state, std::string & value) const {
    bool escape = false;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (enclosing_character == current_character) {
            return NextState {
                pos,
                State::FLUSH_PAIR
            };
        } else if (escape_character == current_character) {
            escape = true;
        } else {
            value.push_back(current_character);
        }
    }

    return NextState {
        pos,
        State::END
    };
}

FSMKeyValuePairFileProcessor::NextState FSMKeyValuePairFileProcessor::flushPair(
        const std::string &file,
        std::size_t pos,
        std::string &key,
        std::string &value,
        std::map<std::string, std::string> &response) const {
    response[std::move(key)] = std::move(value);

    key.clear();
    value.clear();

    return NextState {
        pos,
        pos == file.size() ? State::END : State::WAITING_KEY
    };
}
