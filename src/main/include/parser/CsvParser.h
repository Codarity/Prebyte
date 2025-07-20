#pragma once

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <algorithm>
#include "parser/Parser.h"

namespace prebyte {

class CsvParser : public Parser {
public:
    CsvParser() = default;
    ~CsvParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
