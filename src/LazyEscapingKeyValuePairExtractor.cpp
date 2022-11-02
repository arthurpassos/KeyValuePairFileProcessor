#include "LazyEscapingKeyValuePairExtractor.h"

LazyEscapingKeyValuePairExtractor::LazyEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter,
                                                                     char escape_character,
                                                                     std::optional<char> enclosing_character)
        : KeyValuePairExtractor(item_delimiter, key_value_delimiter, escape_character, enclosing_character) {

}

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readKey(const std::string & file, size_t pos) {
    bool escape = false;

    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == key_value_delimiter) {
            // not checking for empty key because with current waitKey implementation
            // there is no way this piece of code will be reached for the very first key character
            key = createElement(file, start_index, pos - 1);
            return {
                pos,
                State::WAITING_VALUE
            };
        } else if (!std::isalnum(current_character) && current_character != '_') {
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

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEnclosedKey(const std::string &file, size_t pos) {
    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];

        if (enclosing_character == current_character) {
            auto is_key_empty = start_index == pos;

            if (is_key_empty) {
                return {
                    pos,
                    State::WAITING_KEY
                };
            }

            key = createElement(file, start_index, pos - 1);
            return {
                pos,
                State::READING_KV_DELIMITER
            };
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readValue(const std::string &file, size_t pos) {
    bool escape = false;

    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter) {
            value = createElement(file, start_index, pos - 1);
            return {
                pos,
                State::FLUSH_PAIR
            };
        }
    }

    value = createElement(file, start_index, pos);

    // this allows empty values at the end
    return {
        pos,
        State::FLUSH_PAIR
    };
}

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEnclosedValue(const std::string &file, size_t pos) {
    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (enclosing_character == current_character) {
            // not checking for empty value because with current waitValue implementation
            // there is no way this piece of code will be reached for the very first value character
            value = createElement(file, start_index, pos - 1);
            return {
                pos,
                State::FLUSH_PAIR
            };
        }
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEmptyValue(const std::string &file, size_t pos) {
    value = std::string_view();
    return {
        pos + 1,
        State::FLUSH_PAIR
    };
}

KeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::flushPair(const std::string &file, std::size_t pos) {
    response_views[key] = value;

    return {
        pos,
        pos == file.size() ? State::END : State::WAITING_KEY
    };
}

std::string_view LazyEscapingKeyValuePairExtractor::createElement(const std::string & file, std::size_t being, std::size_t end) {
    return std::string_view {file.begin() + being, file.begin() + end};
}

KeyValuePairExtractor::Response LazyEscapingKeyValuePairExtractor::get() const {
    auto unescape = [&](std::string_view element_view) {
        bool escape = false;
        std::string element;

        for (char character : element_view) {
            if (escape) {
                escape = false;
            } else if (character == escape_character) {
                escape = true;
                continue;
            }

            element.push_back(character);
        }

        return element;
    };

    Response response;

    for (auto [key_view, value_view] : response_views) {
        // need to check if RVO applies here or if I'll have to move
        response[unescape(key_view)] = unescape(value_view);
    }

    return response;
}

