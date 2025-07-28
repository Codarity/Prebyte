#pragma once

#include <string>
#include "datatypes/Data.h"
#include "parser/Parser.h"

#include "parser/JsonParser.h"
#include "parser/YamlParser.h"
#include "parser/XmlParser.h"
#include "parser/TomlParser.h"

namespace prebyte {

/**
 * @brief Parses raw string input using a specified parser type.
 *
 * The `StringParser` class provides a high-level interface for parsing structured data
 * (e.g., JSON, YAML, TOML) directly from strings instead of files.
 *
 * It selects the appropriate parser implementation based on the given `ParserType`
 * or parser name and delegates the parsing operation to that backend.
 *
 * This class complements `FileParser`, but is intended for in-memory string data
 * instead of on-disk files.
 */
class StringParser {
public:
    /** @brief Default constructor. Creates a new string-based parser interface. */
    StringParser() = default;

    /**
     * @brief Parses a raw string input using the specified parser type.
     *
     * This is the main entry point for parsing content that is already loaded
     * in memory (e.g., from an HTTP request, embedded config, or CLI input).
     *
     * @param input The structured string content (e.g., JSON, YAML).
     * @param parserType The parser to use for interpretation.
     * @return Parsed `Data` object representing the input content.
     * @throws std::runtime_error if the content is invalid or the parser fails.
     */
    Data parse(const std::string& input, ParserType parserType);

    /**
     * @brief Converts a string like "json" or "yaml" into a `ParserType` enum.
     *
     * Helps resolve a parser type based on user or config input.
     *
     * @param parserName Name of the parser (e.g., "json", "yaml", "toml").
     * @return Corresponding `ParserType` value.
     * @throws std::invalid_argument if the name is not recognized.
     */
    ParserType getParserType(const std::string& parserName) const;

private:
    /**
     * @brief Internal helper that delegates parsing to a specific parser instance.
     *
     * @param input The input string to parse.
     * @param parser Pointer to a parser implementation.
     * @return Parsed `Data` object.
     * @throws std::runtime_error on failure.
     */
    Data parseFile(const std::string& input, Parser* parser);
};

}
