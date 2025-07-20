#pragma once


#include <yaml-cpp/yaml.h>
#include <fstream>
#include <stdexcept>

#include "parser/Parser.h"

namespace prebyte {

class YamlParser : public Parser {
public:
    YamlParser() = default;
    ~YamlParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
