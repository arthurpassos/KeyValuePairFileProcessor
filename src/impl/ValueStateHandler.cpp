#include "ValueStateHandler.h"

ValueStateHandler::ValueStateHandler(char escape_character, char item_delimiter,
                                     std::optional<char> enclosing_character)
    : StateHandler(escape_character, enclosing_character), item_delimiter(item_delimiter)
{}

NextState ValueStateHandler::waitValue(const std::string &file, size_t pos) const {
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
        } else if (std::isalnum(current_character) || current_character == '_') {
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

NextStateWithElement ValueStateHandler::readValue(const std::string &file, size_t pos) const {
    bool escape = false;

    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter || (!std::isalnum(current_character) && current_character != '_')) {
            return {
                {
                    pos,
                    State::FLUSH_PAIR
                },
                createElement(file, start_index, pos - 1)
            };
        }
    }

    // TODO: do I really need the below logic?
    // this allows empty values at the end
    return {
        {
            pos,
            State::FLUSH_PAIR
        },
        createElement(file, start_index, pos)
    };
}

NextStateWithElement ValueStateHandler::readEnclosedValue(const std::string &file, size_t pos) const {
    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (enclosing_character == current_character) {
            // not checking for empty value because with current waitValue implementation
            // there is no way this piece of code will be reached for the very first value character
            return {
                {
                    pos,
                    State::FLUSH_PAIR
                },
                createElement(file, start_index, pos - 1)
            };
        }
    }

    return {
            pos,
            State::END
    };
}

NextStateWithElement ValueStateHandler::readEmptyValue(const std::string &file, size_t pos) const {
    return {
        {
            pos + 1,
            State::FLUSH_PAIR
        },
        {}
    };
}
