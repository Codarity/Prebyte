#pragma once

namespace prebyte {

/**
 * @brief Represents the internal flow control state during script processing.
 *
 * The `FlowState` enum defines the current logical state or directive being handled
 * in the rule engine, macro processor, or similar execution framework. It is used
 * to track block structures such as `if`-statements, `for`-loops, macro definitions,
 * profile declarations, and file inclusions.
 *
 * This enables the interpreter or parser to validate nesting, manage scopes,
 * and respond appropriately to control directives.
 */
enum class FlowState {
    NONE,           /**< No active state; default/neutral state. */

    EXECUTE_MACRO,  /**< Currently executing a macro. */
    DEFINE_PROFILE, /**< Entering a profile definition block. */
    DEFINE_MACRO,   /**< Entering a macro definition block. */
    END_DEFINE,     /**< Marks the end of a macro or profile definition. */

    IF,             /**< Entering an `if` condition block. */
    ELSE_IF,        /**< Entering an `else if` condition block. */
    ELSE,           /**< Entering an `else` fallback block. */
    END_IF,         /**< Closing an `if`/`else if`/`else` block. */

    FOR,            /**< Entering a `for` loop block. */
    END_FOR,        /**< Ending a `for` loop block. */

    INCLUDE         /**< Processing an `include` directive. */
};

}
