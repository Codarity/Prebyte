#pragma once


#include <pugixml.hpp>
#include <fstream>

#include "parser/Parser.h"

namespace prebyte {

class XmlParser : public Parser {
public:
    XmlParser() = default;
    ~XmlParser() override = default;

    Data parse(const std::filesystem::path& filepath) override;
    bool can_parse(const std::filesystem::path& filepath) const override;
};

}
