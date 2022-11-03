#include "KeyValuePairExtractor.h"

#include <optional>

KeyValuePairExtractor::KeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
        : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {

}

KeyValuePairExtractor::Response KeyValuePairExtractor::extract(const std::string & file) {

    auto state = State::WAITING_KEY;

    std::size_t pos = 0;

    while (state != State::END) {
        auto nextState = extract(file, pos, state);

        pos = nextState.pos;
        state = nextState.state;
    }

    return get();
}

KeyValuePairExtractor::NextState KeyValuePairExtractor::extract(const std::string & file, std::size_t pos, State state) {
    switch (state) {
        case State::WAITING_KEY:
            return waitKey(file, pos);
        case State::READING_KEY:
            return readKey(file, pos);
        case State::READING_ENCLOSED_KEY:
            return readEnclosedKey(file, pos);
        case State::READING_KV_DELIMITER:
            return readKeyValueDelimiter(file, pos);
        case State::WAITING_VALUE:
            return waitValue(file, pos);
        case State::READING_VALUE:
            return readValue(file, pos);
        case State::READING_ENCLOSED_VALUE:
            return readEnclosedValue(file, pos);
        case State::READING_EMPTY_VALUE:
            return readEmptyValue(file, pos);
        case State::FLUSH_PAIR:
            return flushPair(file, pos);
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

    // value starts on alpha or enclosing or separator
    // (in this case, it should be empty.. - maybe read empty value to simplify stuff?)

    while (pos < file.size()) {
        const auto current_character = file[pos];

        if (current_character == enclosing_character) {
            return {
                pos + 1u,
                State::READING_ENCLOSED_VALUE
            };
        } else if (current_character == item_delimiter) {
            return {
                pos,
                State::READING_EMPTY_VALUE
            };
        } else if (current_character != SPACE_CHARACTER) {
            return {
                pos,
                State::READING_VALUE
            };
        } else {
            pos++;
        }
    }

    return {
        pos,
        State::READING_EMPTY_VALUE
    };
}
