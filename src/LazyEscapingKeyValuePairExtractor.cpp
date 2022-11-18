#include "LazyEscapingKeyValuePairExtractor.h"

#include <optional>

LazyEscapingKeyValuePairExtractor::LazyEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
        : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {}

LazyEscapingKeyValuePairExtractor::Response LazyEscapingKeyValuePairExtractor::extract(const std::string & file) {

    auto state = State::WAITING_KEY;

    std::size_t pos = 0;

    while (state != State::END) {
        auto nextState = extract(file, pos, state);

        pos = nextState.pos;
        state = nextState.state;
    }

    return get();
}

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::extract(const std::string & file, std::size_t pos, State state) {
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


LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::waitKey(const std::string & file, size_t pos) const {

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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readKeyValueDelimiter(const std::string &file, size_t pos) const {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::waitValue(const std::string &file, size_t pos) const {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readKey(const std::string & file, size_t pos) {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEnclosedKey(const std::string &file, size_t pos) {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readValue(const std::string &file, size_t pos) {
    bool escape = false;

    auto start_index = pos;

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter || (!std::isalnum(current_character) && current_character != '_')) {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEnclosedValue(const std::string &file, size_t pos) {
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

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::readEmptyValue(const std::string &file, size_t pos) {
    value = std::string_view();
    return {
            pos + 1,
            State::FLUSH_PAIR
    };
}

LazyEscapingKeyValuePairExtractor::NextState LazyEscapingKeyValuePairExtractor::flushPair(const std::string &file, std::size_t pos) {
    response_views[key] = value;

    return {
            pos,
            pos == file.size() ? State::END : State::WAITING_KEY
    };
}

std::string_view LazyEscapingKeyValuePairExtractor::createElement(const std::string & file, std::size_t being, std::size_t end) {
    return std::string_view {file.begin() + being, file.begin() + end};
}

LazyEscapingKeyValuePairExtractor::Response LazyEscapingKeyValuePairExtractor::get() const {
    auto unescape = [&](std::string_view element_view) {
        bool escape = false;
        std::string element;

        element.reserve(element_view.size());

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
        response[unescape(key_view)] = unescape(value_view);
    }

    return response;
}