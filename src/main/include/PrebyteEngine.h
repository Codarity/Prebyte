#pragma once

#include <string>
#include <vector>
#include <map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace prebyte {

/**
 * @brief Main API class to use the Prebyte processing engine.
 *
 * Use the `Prebyte` class if you want to programmatically process input (text or files)
 * using Prebyte's variable replacement, macro system, profiles, and rules.
 *
 * You can set variables, apply profiles, define ignore rules or processing rules,
 * and then call `process()` or `process_file()` to generate output.
 *
 * ### Basic usage example:
 * @code
 * Prebyte prebyte;
 * prebyte.set_variable("project", "MyProject");
 * prebyte.set_profile("default");
 * std::string result = prebyte.process("Hello, %%project%%!");
 * @endcode
 *
 * result will be "Hello, MyProject!"
 */
class Prebyte {
public:
    /**
     * @brief Create a Prebyte instance with a specific settings file (e.g. YAML, JSON).
     * @param settings_file Path to the settings file. Supports `~` for home directory.
     *
     * Use this constructor if your application has global or reusable configuration.
     */
    Prebyte(std::string settings_file);

    /**
     * @brief Create a Prebyte instance with no settings file (defaults only).
     *
     * Use this if you're setting everything manually in code.
     */
    Prebyte();

    /** @brief Destructor (no manual cleanup needed). */
    ~Prebyte() = default;

    /**
     * @brief Define a single string variable.
     * @param name Variable name (e.g. `"author"`).
     * @param value String value to assign (e.g. `"Ada"`).
     */
    void set_variable(const std::string& name, const std::string& value);

    /**
     * @brief Define a variable with an empty string value.
     * @param name Variable name.
     */
    void set_variable(const std::string& name);

    /**
     * @brief Define a variable with a list of values (used in loops).
     * @param name Variable name.
     * @param values List of values to assign.
     */
    void set_variable(const std::string& name, std::vector<std::string> values);

    /**
     * @brief Apply a named profile. Profiles may define variables, rules, or ignore lists.
     * @param profile_name Name of the profile (must exist in settings or context).
     */
    void set_profile(const std::string& profile_name);

    /**
     * @brief Mark a rule to be ignored during processing.
     * @param ignore_item Name of the rule or logic to skip.
     */
    void set_ignore(const std::string& ignore_item);

    /**
     * @brief Add or override a custom rule.
     * @param rule_name Name of the rule.
     * @param rule_value Rule expression or logic.
     */
    void set_rule(const std::string& rule_name, const std::string& rule_value);

    /**
     * @brief Process an input string and return the result.
     * @param input Raw input text (e.g., with variables/macros).
     * @return Processed output text.
     *
     * This method is ideal when using the API in memory (e.g., in web services or tests).
     */
    std::string process(const std::string& input);

    /**
     * @brief Process an input file and return the result as string.
     * @param file_path Path to the input file.
     * @return Processed content of the file.
     */
    std::string process_file(const std::string& file_path);

    /**
     * @brief Process an input string and write the result to a file.
     * @param input Raw input text.
     * @param output_path Path to the output file.
     */
    void process(const std::string& input, const std::string& output_path);

    /**
     * @brief Process an input file and write the result to another file.
     * @param file_path Path to the input file.
     * @param output_path Path to the output file.
     */
    void process_file(const std::string& file_path, const std::string& output_path);

private:
    /** @brief Sets up logging based on context and settings. */
    void set_logger();

    /**
     * @brief Expand tilde (`~`) in paths to the user’s home directory.
     * @param path File path (e.g., "~/myfolder/input.txt").
     * @return Expanded absolute path.
     */
    std::string expand_tilde(const std::string& path);
};

}
