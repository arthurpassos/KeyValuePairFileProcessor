#pragma once

#include <string>
#include <optional>
#include "State.h"
#include "StateHandler.h"

class ValueStateHandler : StateHandler {

public:
    ValueStateHandler(char escape_character, char item_delimiter, std::optional<char> enclosing_character);

    [[nodiscard]] NextState wait(const std::string & file, size_t pos) const;
    [[nodiscard]] NextStateWithElement read(const std::string & file, size_t pos) const;
    [[nodiscard]] NextStateWithElement readEnclosed(const std::string & file, size_t pos) const;
    [[nodiscard]] NextStateWithElement readEmpty(const std::string & file, size_t pos) const;

private:
    const char item_delimiter;
};
