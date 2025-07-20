#include "parser/JsonParser.h"


namespace prebyte {

Data convert_json(const nlohmann::json& j) {
    if (j.is_null()) {
        return Data();
    } else if (j.is_boolean()) {
        return Data(j.get<bool>());
    } else if (j.is_number_integer()) {
        return Data(j.get<int>());
    } else if (j.is_number_float()) {
        return Data(j.get<double>());
    } else if (j.is_string()) {
        return Data(j.get<std::string>());
    } else if (j.is_array()) {
        Data::Array arr;
        for (const auto& elem : j) {
            arr.push_back(convert_json(elem));
        }
        return Data(std::move(arr));
    } else if (j.is_object()) {
        Data::Map map;
        for (const auto& [key, value] : j.items()) {
            map[key] = convert_json(value);
        }
        return Data(std::move(map));
    }

    throw std::runtime_error("Unknown JSON type");
}

Data JsonParser::parse(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open JSON file: " + filepath.string());
    }

    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse JSON: " + std::string(e.what()));
    }

    return convert_json(j);
}

bool JsonParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".json") return false;

    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
            nlohmann::json j = nlohmann::json::parse(file, nullptr, true, true);  // allow exceptions
    } catch (...) {
        return false;
    }

    return true;
}

} // namespace prebyte
