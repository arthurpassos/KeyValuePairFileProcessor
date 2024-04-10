#pragma once

#include <unordered_map>
#include <string>
#include <impl/Configuration.h>

/*
TODO    Update docs
 * */
struct KeyValuePairExtractor {
    using Response = std::unordered_map<std::string, std::string>;

    virtual ~KeyValuePairExtractor() = default;

    virtual Response extract(const std::string & file) = 0;

    virtual extractKV::Configuration getConfiguration() const = 0;
};