#include "parser/YamlParser.h"


namespace prebyte {

Data convert_yaml(const YAML::Node& node) {
    if (!node) return Data();

    if (node.IsNull()) {
        return Data();
    } else if (node.IsScalar()) {
        std::string value = node.Scalar();

        if (value == "true" || value == "false") {
            return Data(value == "true");
        }

        static const std::regex int_regex(R"(^-?\d+$)");
        if (std::regex_match(value, int_regex)) {
            return Data(std::stoi(value));
        }

        static const std::regex float_regex(R"(^-?\d+\.\d+$)");
        if (std::regex_match(value, float_regex)) {
            return Data(std::stod(value));
        }

        return Data(value);
    } else if (node.IsSequence()) {
        Data::Array arr;
        for (const auto& elem : node) {
            arr.push_back(convert_yaml(elem));
        }
        return Data(std::move(arr));
    } else if (node.IsMap()) {
        Data::Map map;
        for (const auto& kv : node) {
            const std::string key = kv.first.as<std::string>();
            map[key] = convert_yaml(kv.second);
        }
        return Data(std::move(map));
    }

    throw std::runtime_error("Unsupported YAML node type");
}

Data YamlParser::parse_string(const std::string& yaml_string) {
    try {
        YAML::Node root = YAML::Load(yaml_string);
        return convert_yaml(root);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to parse YAML string: " + std::string(e.what()));
    }
}

Data YamlParser::parse(const std::filesystem::path& filepath) {
    try {
        YAML::Node root = YAML::LoadFile(filepath.string());
        return convert_yaml(root);
    } catch (const YAML::Exception& e) {
        throw std::runtime_error("Failed to parse YAML file: " + std::string(e.what()));
    }
}

bool YamlParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".yaml" && filepath.extension() != ".yml") return false;

    try {
        YAML::Node root = YAML::LoadFile(filepath.string());
        return root.IsDefined();
    } catch (...) {
        return false;
    }
}

} // namespace prebyte
