#include <string>
#include "StateHandler.h"

StateHandler::StateHandler(char escape_character, std::optional<char> enclosing_character)
: escape_character(escape_character), enclosing_character(enclosing_character)
{

}

std::string_view StateHandler::createElement(const std::string & file, std::size_t begin, std::size_t end) const {
    return std::string_view {file.begin() + begin, file.begin() + end};
}
