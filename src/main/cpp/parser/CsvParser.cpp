#include "parser/CsvParser.h"

namespace prebyte {

Data CsvParser::parse(const std::filesystem::path& filepath) {
    Data data;
    return data;
}

bool CsvParser::can_parse(const std::filesystem::path& filepath) const {
    return filepath.extension() == ".csv";
}

}
