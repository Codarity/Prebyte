#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>


#include "datatypes/Context.h"

namespace prebyte {

/**
 * @brief Abstract base class for all processors.
 *
 * The `Processor` class defines the common interface and shared functionality
 * for all processing components in the system (e.g., `Preprocessor`, `Metaprocessor`).
 *
 * It provides protected utility methods for retrieving and parsing variable-related
 * data from the `Context`, and defines the pure virtual `process()` method, which
 * must be implemented by all derived classes.
 */
class Processor {
protected:
    std::unique_ptr<Context> context;  ///< The current execution context.

    /**
     * @brief Retrieves the string value of a variable from the context.
     * @param variable_name Name of the variable to retrieve.
     * @return The variable value as a string.
     * @throws std::runtime_error if the variable is not defined.
     */
    std::string get_variable(const std::string& variable_name) const;

    /**
     * @brief Extracts the variable name from a raw action string.
     * @param action The action string (e.g., `SET_VAR name=value`).
     * @return The extracted variable name.
     */
    std::string get_variable_name(const std::string& action) const;

    /**
     * @brief Extracts the variable value from an action string.
     * @param action The full action string.
     * @param pattern Whether to treat the value as a pattern or literal.
     * @return The extracted variable value.
     */
    std::string get_variable_value(const std::string& action, bool pattern) const;

    /**
     * @brief Extracts and parses a number from a variable assignment.
     * @param action The full action string.
     * @return The numeric value.
     * @throws std::invalid_argument if the value is not a number.
     */
    int get_variable_number(const std::string& action) const;

public:
    /** @brief Virtual destructor for safe polymorphic destruction. */
    virtual ~Processor() = default;

    /**
     * @brief Executes the processor's main logic.
     *
     * This is the central method that must be implemented by all processor types.
     * It defines the behavior for how input is handled, transformed, or responded to.
     */
    virtual void process() = 0;
};

}
