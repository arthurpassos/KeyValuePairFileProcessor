#include "InlineEscapingKeyValuePairExtractor.h"

InlineEscapingKeyValuePairExtractor::InlineEscapingKeyValuePairExtractor(char item_delimiter, char key_value_delimiter,
                                                                         char escape_character,
                                                                         std::optional<char> enclosing_character)
        : KeyValuePairExtractor(item_delimiter, key_value_delimiter, escape_character, enclosing_character) {

}

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::readKey(const std::string & file, size_t pos) {
    bool escape = false;
    key.clear();

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

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::readEnclosedKey(const std::string &file, size_t pos) {
    key.clear();

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (enclosing_character == current_character) {
            return {
                pos,
                State::READING_KV_DELIMITER
            };
        }

        key.push_back(current_character);
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::readValue(const std::string &file, size_t pos) {
    bool escape = false;
    value.clear();

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (escape) {
            escape = false;
            value.push_back(current_character);
        } else if (escape_character == current_character) {
            escape = true;
        } else if (current_character == item_delimiter || (!std::isalnum(current_character) && current_character != '_')) {
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

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::readEnclosedValue(const std::string &file, size_t pos) {

    while (pos < file.size()) {
        const auto current_character = file[pos++];
        if (enclosing_character == current_character) {
            return {
                pos,
                State::FLUSH_PAIR
            };
        }
        value.push_back(current_character);
    }

    return {
        pos,
        State::END
    };
}

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::readEmptyValue(const std::string &file, size_t pos) {
    value.clear();
    return {
        pos,
        State::FLUSH_PAIR
    };
}

KeyValuePairExtractor::NextState InlineEscapingKeyValuePairExtractor::flushPair(const std::string &file, std::size_t pos) {
    response[std::move(key)] = std::move(value);

    key.clear();
    value.clear();

    return {
        pos,
        pos == file.size() ? State::END : State::WAITING_KEY
    };
}

KeyValuePairExtractor::Response InlineEscapingKeyValuePairExtractor::get() const {
    return response;
}

