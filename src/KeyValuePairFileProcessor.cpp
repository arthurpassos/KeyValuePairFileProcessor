#include <stdexcept>
#include <optional>
#include <vector>

#include "KeyValuePairFileProcessor.h"

KeyValuePairFileProcessor::KeyValuePairFileProcessor(char item_delimiter, char key_value_delimiter, char escape_character, std::optional<char> enclosing_character)
    : item_delimiter(item_delimiter), key_value_delimiter(key_value_delimiter), escape_character(escape_character), enclosing_character(enclosing_character) {}

std::map<std::string, std::string> KeyValuePairFileProcessor::process(const std::string & file) const {
    std::map<std::string, std::string> response;

    std::size_t pos = 0;

    try {
        while (pos < file.size()) {
            auto key = getKey(file, pos);

            skipKeyValueDelimiter(file, pos);

            auto value = getValue(file, pos);

            response[key] = value;
        }
    } catch (const std::exception &) {
    }

    return response;
}

std::string KeyValuePairFileProcessor::getKey(const std::string & file, std::size_t & pos) const {
    std::vector<char> word;
    bool escape = false;
    bool enclose_start_found = false;

    while (pos < file.size()) {
        char current_char = file[pos];

        if (escape) {
            word.push_back(current_char);
            escape = false;
        } else if (enclosing_character && enclosing_character == current_char) {
            if (enclose_start_found) {
                pos++;
                break;
            } else {
                enclose_start_found = true;
            }
        } else if (current_char == SPACE_CHARACTER && !enclose_start_found) {
            word.clear();
        } else if (current_char == escape_character) {
            escape = true;
        } else if (current_char == key_value_delimiter) {
            break;
        } else {
            word.push_back(current_char);
        }

        pos++;
    }

    if (word.empty()) {
        throw std::runtime_error("Empty key");
    }

    return std::string(word);
}

std::string KeyValuePairFileProcessor::getValue(const std::string & file, std::size_t & pos) const {
    // trim left
    while (pos < file.size() && file[pos] == SPACE_CHARACTER) {
        pos++;
    }

    std::vector<char> word;

    while (pos < file.size()) {
        char current_char = file[pos++];
        if (current_char == item_delimiter) {
            break;
        } else {
            word.push_back(current_char);
        }
    }

    return std::string(word);
}

void KeyValuePairFileProcessor::skipKeyValueDelimiter(const std::string & file, std::size_t & pos) const {
    if (pos + 1u >= file.size()) {
        throw std::runtime_error("Expected: " + std::string(1, key_value_delimiter) + " but found EOF\n");
    }

    skipElement(key_value_delimiter, file, pos);
}

void KeyValuePairFileProcessor::skipItemDelimiter(const std::string & file, std::size_t & pos) const {
    if (pos + 1u > file.size()) {
        throw std::runtime_error("Expected: " + std::string(1, item_delimiter) + " but found EOF\n");
    }

    skipElement(item_delimiter, file, pos);
}

void KeyValuePairFileProcessor::skipElement(char item, const std::string & file, std::size_t & pos) const {
    if (item != file[pos]) {
        throw std::runtime_error("Excepted " + std::string(1, item) + ", found: " + std::string(1, file[pos]));
    }

    pos++;
}