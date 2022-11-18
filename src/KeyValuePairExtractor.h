#pragma once

#include <unordered_map>
#include <string>

struct KeyValuePairExtractor {
    using Response = std::unordered_map<std::string, std::string>;

    virtual ~KeyValuePairExtractor() = default;

    virtual Response extract(const std::string & file) = 0;

};