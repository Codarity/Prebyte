#pragma once

#include <fstream>
#include <nlohmann/json.hpp>

#include "parser/Parser.h"

namespace prebyte {

class JsonParser : public Parser {
public:
    JsonParser() = default;
    ~JsonParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
