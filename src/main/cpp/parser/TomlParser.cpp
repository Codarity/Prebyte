#include "parser/TomlParser.h"

namespace prebyte {

Data TomlParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool TomlParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".toml";
}

}
