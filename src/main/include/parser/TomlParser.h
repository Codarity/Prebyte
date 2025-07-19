#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class TomlParser : public FileParser {
public:
    TomlParser() = default;
    ~TomlParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
