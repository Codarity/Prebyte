#include "parser/StringParser.h"

namespace prebyte {

Data StringParser::parse(const std::string& input, ParserType parserType) {
        Parser* parser;

        switch (parserType) {
                case ParserType::JSON:
                        parser = new JsonParser();
                        break;
                case ParserType::YAML:
                        parser = new YamlParser();
                        break;
                case ParserType::TOML:
                        parser = new TomlParser();
                        break;
                default:
                        throw std::runtime_error("Unsupported parser type");
        }
        try {
                return parser->parse_string(input);
        } catch (const std::exception& e) {
                throw std::runtime_error("Error parsing input: " + std::string(e.what()));
        }
}


ParserType StringParser::getParserType(const std::string& parserName) const {
        if (parserName == "json") {
                return ParserType::JSON;
        } else if (parserName == "yaml" || parserName == "yml") {
                return ParserType::YAML;
        } else if (parserName == "xml") {
                return ParserType::XML;
        } else if (parserName == "toml") {
                return ParserType::TOML;
        } else if (parserName == "csv") {
                return ParserType::CSV;
        } else if (parserName == "ini" || parserName == "cfg") {
                return ParserType::INI;
        } else if (parserName == "env") {
                return ParserType::ENV;
        }
        throw std::runtime_error("Unknown parser type: " + parserName);
}

}
