#include "parser/TomlParser.h"


namespace prebyte {

Data convert_toml_value(const std::shared_ptr<cpptoml::base>& val);

Data convert_array(const std::shared_ptr<cpptoml::array>& arr) {
    Data::Array array;
    for (const auto& elem : *arr) {
        array.push_back(convert_toml_value(elem));
    }
    return Data(array);
}

Data convert_table(const std::shared_ptr<cpptoml::table>& table) {
    Data::Map map;
    for (const auto& [key, value] : *table) {
        map[key] = convert_toml_value(value);
    }
    return Data(map);
}

Data convert_toml_value(const std::shared_ptr<cpptoml::base>& val) {
    if (auto v = val->as<std::string>()) {
        return Data(v->get());
    } else if (auto v = val->as<int64_t>()) {
        return Data(static_cast<int>(v->get()));
    } else if (auto v = val->as<double>()) {
        return Data(v->get());
    } else if (auto v = val->as<bool>()) {
        return Data(v->get());
    } else if (auto v = val->as_array()){
        return convert_array(v);
    } else if (auto v = val->as_table()) {
        return convert_table(v);
    }

    return Data();  // fallback null
}

Data TomlParser::parse(const std::filesystem::path& filepath) {
    try {
        auto table = cpptoml::parse_file(filepath.string());
        return convert_table(table);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to parse TOML: " + std::string(e.what()));
    }
}

bool TomlParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".toml") return false;

    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    try {
        cpptoml::parse_file(filepath.string());
    } catch (...) {
        return false;
    }

    return true;
}

Data TomlParser::parse_string(const std::string& input) {
        try {
                std::istringstream input_stream(input);
                auto table = cpptoml::parser(input_stream).parse();
                return convert_table(table);
        } catch (const std::exception& e) {
                throw std::runtime_error("Failed to parse TOML string: " + std::string(e.what()));
        }
}

} // namespace prebyte
