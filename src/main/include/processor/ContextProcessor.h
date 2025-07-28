#pragma once

#include <filesystem>

#include "datatypes/ActionType.h"
#include "datatypes/CliStruct.h"
#include "datatypes/Context.h"
#include "datatypes/Data.h"
#include "datatypes/Profile.h"
#include "datatypes/Rules.h"
#include "parser/FileParser.h"

namespace prebyte {

/**
 * @brief Processes CLI input and configuration data into a ready-to-use execution context.
 *
 * The `ContextProcessor` is responsible for initializing and preparing the `Context`
 * based on information provided via the `CliStruct`. It loads settings, profiles,
 * variables, ignore lists, and rules from various sources (e.g., CLI, files, defaults),
 * and configures the logger and runtime behavior accordingly.
 *
 * This class effectively bridges CLI parsing and actual program execution by transforming
 * user input into structured runtime state.
 */
class ContextProcessor {
private:
    CliStruct cli_struct;                      ///< Parsed CLI state and raw input.
    std::unique_ptr<Context> context;          ///< Execution context to populate and return.
    ActionType action_type;                    ///< Determined action type (e.g. HELP, EXPLAIN, FILE_IN_FILE_OUT).

    /** @brief Determines the action type to be executed based on CLI state. */
    void determine_action_type();

    /** @brief Loads and applies settings from the settings file or defaults. */
    void load_settings();

    /** @brief Loads profile definitions from CLI or settings. */
    void load_profiles();

    /** @brief Loads user-defined or configured variables into the context. */
    void load_variables();

    /** @brief Loads ignore rules (rules to skip). */
    void load_ignore();

    /** @brief Loads rules into the context from CLI, settings, or profiles. */
    void load_rules();

    /**
     * @brief Overload: Loads rules from a key-value map of rule names to rule content.
     * @param rules A map of rule names and corresponding rule values.
     */
    void load_rules(const std::map<std::string, std::string>& rules);

    /** @brief Loads and verifies the action type defined in the context or input. */
    void load_action_type();

    /**
     * @brief Injects variables from a given file into the context.
     * @param filePath Path to a file containing variable definitions.
     */
    void inject_variables(const std::string& filePath);

    /**
     * @brief Searches for a settings file starting from the given path.
     * @param path Starting path for the search.
     * @return Optional path to the settings file if found.
     */
    std::optional<std::filesystem::path> find_settings_file(const std::filesystem::path& path) const;

    /**
     * @brief Converts a `Data` object into a variables map.
     * @param variables Structured data containing variable definitions.
     * @return A map of variable names to their values.
     */
    std::map<std::string, std::vector<std::string>> get_variables(const Data& variables);

    /**
     * @brief Converts a `Data` object into a profiles map.
     * @param profiles Structured data containing profile definitions.
     * @return A map of profile names to profile objects.
     */
    std::map<std::string, Profile> get_profiles(const Data& profiles);

    /**
     * @brief Extracts ignore rules from a `Data` object.
     * @param ignore Structured data listing ignore targets.
     * @return A set of rules or entries to ignore.
     */
    std::unordered_set<std::string> get_ignore(const Data& ignore);

    /**
     * @brief Extracts rules from a `Data` object.
     * @param rules Structured data listing rule definitions.
     * @return A map of rule names to rule content.
     */
    std::map<std::string, std::string> get_rules(const Data& rules);

    /** @brief Initializes the logger based on CLI or settings. */
    void set_logger();

    /**
     * @brief Expands `~` in file paths to the user's home directory.
     * @param path Path that may contain `~`.
     * @return Expanded absolute path.
     */
    std::string expand_tilde(const std::string& path);

    /** @brief Extracts a string from a `Data` object. */
    std::string get_string(Data data) const;

    /** @brief Extracts an integer from a `Data` object. */
    int get_int(Data data) const;

    /** @brief Extracts a double from a `Data` object. */
    double get_double(Data data) const;

    /**
     * @brief Merges a map of rule definitions into an existing `Rules` object.
     * @param target The target rule set to be updated.
     * @param source Source rules to merge.
     */
    void merge_rules(Rules& target, const std::map<std::string, std::string>& source);

public:
    /**
     * @brief Constructs a new `ContextProcessor`.
     * @param cli_struct Parsed CLI structure.
     * @param context Pre-initialized context object to populate.
     */
    ContextProcessor(const CliStruct& cli_struct, std::unique_ptr<Context> context);

    /**
     * @brief Processes all CLI/config/profile input and returns a ready-to-use context.
     *
     * This is the main entry point for preparing program state before execution.
     *
     * @return A fully populated and configured execution context.
     */
    std::unique_ptr<Context> process();
};

}  // namespace prebyte
