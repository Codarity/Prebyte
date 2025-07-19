#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class CsvParser : public FileParser {
public:
    CsvParser() = default;
    ~CsvParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
