#include "parser/XmlParser.h"

namespace prebyte {

Data XmlParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool XmlParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".xml";
}

}
