#include "parser/EnvParser.h"

namespace prebyte {

Data EnvParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool EnvParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".env";
}

}
