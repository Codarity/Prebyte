#pragma once

#include <string>
#include "datatypes/Data.h"
#include "parser/Parser.h"

#include "parser/JsonParser.h"
#include "parser/YamlParser.h"
#include "parser/XmlParser.h"
#include "parser/TomlParser.h"

namespace prebyte {

class StringParser {
public:
        StringParser() = default;
        Data parse(const std::string& filePath, ParserType parserType);
        ParserType getParserType(const std::string& parserName) const;
private:
        Data parseFile(const std::string& filePath, Parser* parser);
};

}
