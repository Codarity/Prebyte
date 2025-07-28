#pragma once

#include <filesystem>
#include "datatypes/Data.h"

namespace prebyte {

/**
 * @brief Represents the supported types of file parsers.
 *
 * This enumeration defines the formats that can be parsed by the system.
 * It is typically used to select or identify the appropriate `Parser`
 * implementation based on file extension or user preference.
 */
enum ParserType {
    JSON,  /**< JavaScript Object Notation (*.json) */
    XML,   /**< XML (Extensible Markup Language) (*.xml) */
    YAML,  /**< YAML Ain’t Markup Language (*.yaml, *.yml) */
    CSV,   /**< Comma-Separated Values (*.csv) */
    INI,   /**< INI configuration files (*.ini) */
    ENV,   /**< Environment variable style files (.env) */
    TOML   /**< TOML configuration files (*.toml) */
};

/**
 * @brief Abstract interface for file content parsers.
 *
 * The `Parser` class defines a generic interface for reading structured data
 * from files or strings and converting it into a unified `Data` representation.
 *
 * Concrete subclasses (e.g., `JsonParser`, `YamlParser`, etc.) implement the
 * parsing logic for specific formats. This interface allows the system to
 * support multiple file types through a common API, facilitating plug-and-play
 * parser strategies and format detection.
 */
class Parser {
public:
    /** @brief Virtual destructor for safe polymorphic destruction. */
    virtual ~Parser() = default;

    /**
     * @brief Parses the contents of a file into a `Data` structure.
     *
     * @param filepath Path to the file to parse.
     * @return Parsed `Data` object.
     * @throws std::runtime_error or implementation-defined errors on failure.
     */
    virtual Data parse(const std::filesystem::path& filepath) = 0;

    /**
     * @brief Parses raw string content into a `Data` structure.
     *
     * This can be used to parse inline or dynamically constructed strings
     * without reading from a file.
     *
     * @param input The string to parse.
     * @return Parsed `Data` object.
     * @throws std::runtime_error or implementation-defined errors on failure.
     */
    virtual Data parse_string(const std::string& input) = 0;

    /**
     * @brief Checks whether the parser supports a given file based on its path.
     *
     * This is typically used to check file extensions or header content before parsing.
     *
     * @param filepath Path to the file in question.
     * @return `true` if this parser can handle the file; `false` otherwise.
     */
    virtual bool can_parse(const std::filesystem::path& filepath) const = 0;
};

}
