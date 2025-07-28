#pragma once

#include <spdlog/common.h>
#include <string>
#include <optional>
#include <filesystem>

#include <spdlog/spdlog.h>

#include "datatypes/Data.h"

namespace prebyte {

/**
 * @brief Enumeration of available benchmarking modes.
 *
 * This enum defines which aspects of benchmarking should be performed
 * during execution.
 */
enum class Benchmark {
    NONE,   /**< No benchmarking is performed. */
    TIME,   /**< Measure execution time. */
    MEMORY, /**< Measure memory usage. */
    ALL     /**< Enable all available benchmarks. */
};


/**
 * @brief Represents a set of rules that control runtime behavior and configuration.
 *
 * This struct holds various optional configuration flags and values that influence how
 * the system interprets variables, includes, formatting, and logging.
 * 
 * Rules can be dynamically loaded (e.g. from profiles or config files) and provide fine-grained
 * control over parsing behavior and system features such as environment variable access or
 * debug logging levels.
 */
struct Rules {
    std::optional<bool> strict_variables;        /**< If true, undefined variables will raise an error. */
    std::optional<bool> set_default_variables;   /**< If true, sets default values for missing variables. */
    std::optional<bool> trim_start;              /**< If true, trims whitespace from the start of variable values. */
    std::optional<bool> trim_end;                /**< If true, trims whitespace from the end of variable values. */
    std::optional<bool> allow_env;               /**< Allows reading values from environment variables. */
    std::optional<bool> allow_env_fallback;      /**< Uses environment variables as fallback if value is not set. */
    std::optional<spdlog::level::level_enum> debug_level; /**< Sets the logging verbosity level (mapped to spdlog). */
    std::optional<int> max_variable_length;      /**< Maximum allowed length for variables (-1 = unlimited). */
    std::optional<std::string> default_variable_value; /**< Value to use when a variable is missing and defaulting is enabled. */
    std::optional<std::string> variable_prefix;  /**< Optional prefix for variables (e.g., `$`). */
    std::optional<std::string> variable_suffix;  /**< Optional suffix for variables (e.g., `}` or `]`). */
    std::optional<std::string> include_path;     /**< Directory path used to resolve includes. */
    std::optional<Benchmark> benchmark;          /**< Benchmarking mode (time, memory, both, or none). */

    /**
     * @brief Registers a rule from its name and associated data.
     *
     * Parses and applies a rule from an external data source (e.g., configuration file or profile).
     *
     * @param rule_name The name of the rule to apply.
     * @param rule_data The value or structure representing the rule's content.
     * @return The determined logging level after applying this rule (if relevant).
     */
    spdlog::level::level_enum add_rule(std::string rule_name, const Data& rule_data);

    /**
     * @brief Extracts a string from a `Data` object.
     * @param data The data to extract from.
     * @return The string value.
     * @throws std::bad_variant_access if the type is incorrect.
     */
    std::string get_string(Data data);

    /**
     * @brief Extracts an integer from a `Data` object.
     * @param data The data to extract from.
     * @return The integer value.
     * @throws std::bad_variant_access if the type is incorrect.
     */
    int get_int(Data data);

    /**
     * @brief Extracts a double from a `Data` object.
     * @param data The data to extract from.
     * @return The double value.
     * @throws std::bad_variant_access if the type is incorrect.
     */
    double get_double(Data data);

    /**
     * @brief Initializes or resets the internal state of the ruleset.
     *
     * This function can be used to apply defaults or clear previously set values.
     */
    void init();
};


}
