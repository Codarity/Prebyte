#include "parser/EnvParser.h"


namespace prebyte {

Data EnvParser::parse(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }

    Data::Map env_data;
    std::string line;

    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#') continue;

        auto equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            throw std::runtime_error("Invalid line (no '=') in .env: " + line);
        }

        std::string key = line.substr(0, equal_pos);
        std::string value = line.substr(equal_pos + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        env_data[key] = value;
    }

    return env_data;
}

bool EnvParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".env") return false;

    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty() || line[0] == '#') continue;

        auto equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            return false;
        }
    }

    return true;
}

} // namespace prebyte
