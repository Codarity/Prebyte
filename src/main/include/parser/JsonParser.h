#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class JsonParser : public FileParser {
public:
    JsonParser() = default;
    ~JsonParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
