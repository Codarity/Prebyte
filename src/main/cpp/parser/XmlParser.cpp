#include "parser/XmlParser.h"

namespace prebyte {

Data convert_xml_node(const pugi::xml_node& node) {
    if (!node) return Data();

    if (node.type() == pugi::node_pcdata || node.type() == pugi::node_cdata) {
        return std::string(node.value());
    }

    Data::Map map;

    for (auto attr : node.attributes()) {
        map["@" + std::string(attr.name())] = std::string(attr.value());
    }

    std::string text = node.child_value();
    if (!text.empty() && node.first_child().type() == pugi::node_pcdata) {
        map["_text"] = text;
    }

    for (auto child : node.children()) {
        std::string name = child.name();
        if (name.empty()) continue;

        if (map.find(name) == map.end()) {
            map[name] = convert_xml_node(child);
        } else {
            Data existing = map[name];
            if (!existing.is_array()) {
                Data::Array arr = { existing };
                existing = Data(std::move(arr));
            }

            Data::Array copy = existing.as_array();
            copy.push_back(convert_xml_node(child));
            map[name] = Data(std::move(copy));
        }
    }

    return Data(std::move(map));
}

Data XmlParser::parse(const std::filesystem::path& filepath) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    if (!result) {
        throw std::runtime_error("Failed to parse XML: " + std::string(result.description()));
    }

    return convert_xml_node(doc.document_element());
}

bool XmlParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".xml") return false;

    pugi::xml_document doc;
    return doc.load_file(filepath.c_str());
}

} // namespace prebyte
