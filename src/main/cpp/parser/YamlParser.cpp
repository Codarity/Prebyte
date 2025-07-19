#include "parser/YamlParser.h"

namespace prebyte {

Data YamlParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool YamlParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".yaml" || filepath.extension() == ".yml";
}

}
