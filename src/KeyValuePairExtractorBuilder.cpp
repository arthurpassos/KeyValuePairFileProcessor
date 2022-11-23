#include "KeyValuePairExtractorBuilder.h"

#include <utility>
#include "impl/LazyEscapingKeyValuePairExtractor.h"

KeyValuePairExtractorBuilder &KeyValuePairExtractorBuilder::withKeyValuePairDelimiter(char key_value_pair_delimiter_) {
    key_value_pair_delimiter = key_value_pair_delimiter_;
    return *this;
}

KeyValuePairExtractorBuilder &KeyValuePairExtractorBuilder::withEscapeCharacter(char escape_character_) {
    escape_character = escape_character_;
    return *this;
}

KeyValuePairExtractorBuilder &KeyValuePairExtractorBuilder::withItemDelimiter(char item_delimiter_) {
    item_delimiter = item_delimiter_;
    return *this;
}

KeyValuePairExtractorBuilder & KeyValuePairExtractorBuilder::withEnclosingCharacter(std::optional<char> enclosing_character_) {
    enclosing_character = enclosing_character_;
    return *this;
}

KeyValuePairExtractorBuilder &KeyValuePairExtractorBuilder::withValueSpecialCharacterAllowList(std::unordered_set<char> value_special_character_allowlist_) {
    value_special_character_allowlist = std::move(value_special_character_allowlist_);
    return *this;
}

std::shared_ptr<KeyValuePairExtractor> KeyValuePairExtractorBuilder::build() {
    KeyStateHandler keyStateHandler(key_value_pair_delimiter, escape_character, enclosing_character);
    ValueStateHandler valueStateHandler(escape_character, item_delimiter, enclosing_character, value_special_character_allowlist);
    KeyValuePairEscapingProcessor escapingProcessor(escape_character);

    return std::make_shared<LazyEscapingKeyValuePairExtractor>(keyStateHandler, valueStateHandler, escapingProcessor);
}
