#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <sstream>
#include <stack>
#include <regex>

#include "processor/FlowState.h"
#include "datatypes/Context.h"

namespace prebyte {

/**
 * @brief Enumeration of supported flow control and transformation actions.
 *
 * `FlowType` defines all recognized preprocessing actions that the engine can execute,
 * including setting variables, profiles, macros, conditionals, loops, and includes.
 */
enum class FlowType {
    SET_VAR,         /**< Sets a variable to a value. */
    UNSET_VAR,       /**< Removes a variable from the current scope. */
    SET_RULE,        /**< Adds or overrides a rule. */
    DEFINE_PROFILE,  /**< Starts a new profile definition block. */
    SET_PROFILE,     /**< Applies an existing profile to the current context. */
    UNSET_PROFILE,   /**< Removes an active profile from context. */
    SET_IGNORE,      /**< Adds entries to the ignore list. */
    UNSET_IGNORE,    /**< Removes entries from the ignore list. */
    DEFINE_MACRO,    /**< Begins definition of a new macro. */
    END_DEFINE,      /**< Marks the end of a macro or profile definition. */
    EXECUTE_MACRO,   /**< Executes a previously defined macro. */
    IF,              /**< Starts an IF conditional block. */
    ELSE_IF,         /**< Starts an ELSE IF condition. */
    ELSE,            /**< Starts an ELSE block. */
    ENDIF,           /**< Ends an IF/ELSE control block. */
    FOR,             /**< Starts a FOR loop block. */
    ENDFOR,          /**< Ends a FOR loop block. */
    INCLUDE          /**< Includes and processes an external file. */
};


/**
 * @brief Handles execution of flow-related preprocessing actions (e.g., conditionals, loops, macros).
 *
 * The `ProcessingFlow` class interprets and evaluates control flow logic embedded
 * in the input stream, including:
 * - Conditional evaluation (IF, ELSE, ELSE IF)
 * - Looping (FOR)
 * - Macro definition and execution
 * - Profile and variable manipulation
 * - Includes
 *
 * This class works closely with the current `Context` to apply changes to runtime
 * state as flow actions are processed.
 */
class ProcessingFlow {
private:
    Context* context;       ///< Pointer to the current execution context.
    FlowState flow_state;   ///< Current high-level flow state (e.g., inside IF, FOR, MACRO, etc.).

    /** @brief Handles the SET_VAR action. */
    std::string _SET_VAR(const std::string& action);

    /** @brief Handles the UNSET_VAR action. */
    std::string _UNSET_VAR(const std::string& action);

    /** @brief Handles the SET_RULE action. */
    std::string _SET_RULE(const std::string& action);

    /** @brief Begins a profile definition block. */
    std::string _DEFINE_PROFILE(const std::string& action);

    /** @brief Applies a named profile to the current context. */
    std::string _SET_PROFILE(const std::string& action);

    /** @brief Removes a profile from the current context. */
    std::string _UNSET_PROFILE(const std::string& action);

    /** @brief Adds entries to the ignore list. */
    std::string _SET_IGNORE(const std::string& action);

    /** @brief Removes entries from the ignore list. */
    std::string _UNSET_IGNORE(const std::string& action);

    /** @brief Begins a macro definition block. */
    std::string _DEFINE_MACRO(const std::string& action);

    /** @brief Executes a defined macro. */
    std::string _EXECUTE_MACRO(const std::string& action);

    /** @brief Begins an IF block and evaluates its condition. */
    std::string _IF(const std::string& action);

    /** @brief Evaluates an ELSE IF condition. */
    std::string _ELSE_IF(const std::string& action);

    /** @brief Handles the ELSE block. */
    std::string _ELSE(const std::string& action);

    /** @brief Begins a FOR loop. */
    std::string _FOR(const std::string& action);

    /** @brief Handles the INCLUDE action. */
    std::string _INCLUDE(const std::string& action);

    /** @brief Evaluates the truth value of a given expression. */
    bool is_true(const std::string& action) const;

    /** @brief Evaluates an OR expression. */
    bool eval_or(const std::string& expr) const;

    /** @brief Evaluates an AND expression. */
    bool eval_and(const std::string& expr) const;

    /** @brief Evaluates a NOT expression. */
    bool eval_not(const std::string& expr) const;

    /** @brief Evaluates a comparison expression (==, !=, <, etc.). */
    bool eval_comparison(const std::string& expr) const;

    /** @brief Retrieves a string value from a token, resolving variables if needed. */
    std::string get_str_value(const std::string& token) const;

    /** @brief Trims whitespace from a string. */
    std::string trim(const std::string& s) const;

public:
    /**
     * @brief Constructs a new `ProcessingFlow` tied to a context.
     * @param context Pointer to the execution context.
     */
    ProcessingFlow(Context* context) : context(context) {}

    /**
     * @brief Determines whether the given action is a valid flow instruction.
     * @param action The action string to check.
     * @return `true` if the action is recognized; otherwise `false`.
     */
    bool is_valid(const std::string& action) const;

    /**
     * @brief Processes a flow action and returns its output or effect.
     * @param action The action string to execute.
     * @return Resulting string output (may be empty).
     */
    std::string get_value(const std::string& action);

    /**
     * @brief Returns the current `FlowState`.
     * @return The flow state currently being processed.
     */
    FlowState get_flow_state() const;

    /**
     * @brief Checks whether a given action should be skipped based on control flow.
     * @param action The action to evaluate.
     * @param ignore Whether to ignore the current block (e.g., due to false condition).
     * @return `true` if the action should be skipped; otherwise `false`.
     */
    bool is_skipable(const std::string& action, bool ignore) const;
};

}
