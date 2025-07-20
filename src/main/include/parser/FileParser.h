#pragma once

#include <string>
#include <filesystem>

#include "datatypes/Data.h"
#include "parser/Parser.h"
#include "parser/JsonParser.h"
#include "parser/XmlParser.h"
#include "parser/YamlParser.h"
#include "parser/CsvParser.h"
#include "parser/IniParser.h"
#include "parser/EnvParser.h"
#include "parser/TomlParser.h"

namespace prebyte {

class FileParser {
public:
        FileParser() = default;
        Data parse(const std::string& filePath);
private:
        Data parseFile(const std::string& filePath, Parser* parser);
};

}
