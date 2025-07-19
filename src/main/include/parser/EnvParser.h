#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class EnvParser : public FileParser {
public:
    EnvParser() = default;
    ~EnvParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
