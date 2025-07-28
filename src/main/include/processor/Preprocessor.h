#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stack>

#include "processor/Processor.h"
#include "processor/ProcessingVariables.h"
#include "processor/ProcessingFlow.h"
#include "datatypes/Context.h"
#include "processor/FlowState.h"
#include "parser/YamlParser.h"
#include "datatypes/Profile.h"
#include "parser/StringParser.h"
#include "datatypes/Rules.h"

namespace prebyte {

/**
 * @brief Executes preprocessing logic such as macro expansion, flow control, variable resolution, and includes.
 *
 * The `Preprocessor` class is the main engine for interpreting and transforming structured input.
 * It handles advanced preprocessing features like:
 * - Macro execution
 * - Profile inclusion and expansion
 * - Conditional logic (if / else / for)
 * - Variable injection and substitution
 * - File includes and recursion tracking
 * - Flow control using a stack of `FlowState`
 * - Benchmarking and output construction
 *
 * It inherits from `Processor` and implements the `process()` method as its primary entry point.
 */
class Preprocessor : public Processor {
private:
    std::string input;                             ///< The input text or content to be processed.
    std::string output;                            ///< The final output after preprocessing.
    ProcessingVariables process_variables;         ///< Internal structure managing runtime variable state.
    ProcessingFlow process_flow;                   ///< Internal structure managing flow control state.
    std::stack<FlowState> flow_states;             ///< Stack to track nested flow states (e.g. IF, FOR).
    std::vector<std::filesystem::path> processed_includes;  ///< Tracks included files to prevent duplicates.
    
    bool ignore_next = false;                      ///< Indicates whether the next line should be ignored.
    ushort current_depth = 0;                      ///< Current depth of nested includes or macros.
    ushort ignore_all = 0;                         ///< Counter used to ignore entire blocks (e.g. during IF false).
    ushort ignore_depth = 0;                       ///< Tracks how deeply we are in ignored structures.
    
    bool pipe = false;                             ///< Whether output should be piped into another processor.
    
    std::string profile_name;                      ///< Current profile name, if applicable.
    std::string profile_parser_type;               ///< Parser type used to load the profile.
    std::string macro_name;                        ///< Current macro being executed.
    std::string for_variable;                      ///< Variable used in current FOR loop.
    int for_stack = 0;                             ///< Nesting depth of FOR loops.
    std::stack<std::vector<std::string>> macro_args; ///< Stack of macro arguments per invocation.

    /** @brief Retrieves the raw input to process. */
    std::string get_input() const;

    /** @brief Builds the final output string. */
    void make_output() const;

    /**
     * @brief Main preprocessing loop that handles macro expansion, conditionals, includes, etc.
     * @param input The raw input string to process.
     * @return Fully processed output.
     */
    std::string process_all(std::string input);

    /**
     * @brief Executes a single preprocessing action or directive.
     * @param action The action to perform (e.g. macro, control structure).
     * @return Output generated from the action.
     */
    std::string do_action(const std::string& action);

    /**
     * @brief Handles flow-control actions (IF, FOR, ELSE, etc.).
     * @param action The flow control action string.
     * @return Result of the action (may be empty).
     */
    std::string process_code_flow(const std::string& action);

    /**
     * @brief Adds a string to the output, applying preprocessing logic if needed.
     * @param output Target output string.
     * @param str String to add.
     */
    void add_string(std::string& output, const std::string& str);

    /** @brief Prints benchmark information such as execution time. */
    void make_benchmark() const;

    /** @brief Extracts rule definitions from a `Data` object. */
    std::map<std::string, std::string> get_rules(const Data& rules);

    /** @brief Extracts ignore list from a `Data` object. */
    std::unordered_set<std::string> get_ignore(const Data& ignore);

    /** @brief Extracts variable definitions from a `Data` object. */
    std::map<std::string, std::vector<std::string>> get_variables(const Data& variables);

    /**
     * @brief Retrieves the list of values for a variable by name.
     * @param variable Variable name.
     * @return List of values assigned to the variable.
     */
    std::vector<std::string> get_variable_values(std::string variable);

    /**
     * @brief Converts a time duration to a readable string format.
     * @param duration Duration in nanoseconds.
     * @return Human-readable time string.
     */
    std::string get_time_conversion(const std::chrono::nanoseconds& duration) const;

public:
    /**
     * @brief Constructs a `Preprocessor` from a given context.
     * @param context Execution context containing configuration, rules, and input data.
     */
    Preprocessor(std::unique_ptr<Context> context);

    /**
     * @brief Executes preprocessing based on the current context and input.
     *
     * This method is the main entry point and overrides `Processor::process()`.
     * It performs macro processing, rule application, variable substitution,
     * and builds the final output.
     */
    void process() override;
};

}
