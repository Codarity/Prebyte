#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class YamlParser : public FileParser {
public:
    YamlParser() = default;
    ~YamlParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
