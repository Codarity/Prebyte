#pragma once


#include <fstream>
#include <stdexcept>
#include <filesystem>

#include "parser/Parser.h"

namespace prebyte {

class EnvParser : public Parser {
public:
    EnvParser() = default;
    ~EnvParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
