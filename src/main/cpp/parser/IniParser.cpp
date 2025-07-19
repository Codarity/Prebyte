#include "parser/IniParser.h"

namespace prebyte {

Data IniParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool IniParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".ini" || filepath.extension() == ".cfg";
}

}
