#include "parser/JsonParser.h"

namespace prebyte {

Data JsonParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool JsonParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".json";
}

}
