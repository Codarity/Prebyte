#pragma once

#include "parser/FileParser.h"

namespace prebyte {

class XmlParser : public FileParser {
public:
    XmlParser() = default;
    ~XmlParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
