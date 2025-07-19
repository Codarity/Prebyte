#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class IniParser : public FileParser {
public:
    IniParser() = default;
    ~IniParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
