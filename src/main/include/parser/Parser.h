#pragma once

#include <filesystem>
#include "datatypes/Data.h"

namespace prebyte {

enum ParserType {
        JSON,
        XML,
        YAML,
        CSV,
        INI,
        ENV,
        TOML
};

class Parser {
public:
    virtual ~Parser() = default;

    virtual Data parse(const std::filesystem::path& filepath) = 0;
    virtual Data parse_string(const std::string& input) = 0;

    virtual bool can_parse(const std::filesystem::path& filepath) const = 0;
};

}
