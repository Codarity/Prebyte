#pragma once

#include <string>
#include <iostream>

#include "processor/Processor.h"


extern const std::string VERSION;

namespace prebyte {

/**
 * @brief Processor for handling meta-level commands like help, version, and listings.
 *
 * The `Metaprocessor` handles non-transformational actions such as showing help screens,
 * version information, listing rules or variables, or printing explanations.
 *
 * It inherits from the base `Processor` interface and overrides the `process()` method
 * to execute the meta-level action determined by the provided `Context`.
 *
 * This class is typically invoked when the CLI input requests information instead
 * of performing data transformation.
 */
class Metaprocessor : public Processor {
private:
    /** @brief Displays a detailed explanation of the rule logic or CLI action. */
    void explain();

    /** @brief Displays basic usage information for the CLI tool. */
    void help();

    /** @brief Displays advanced or extended help with full details. */
    void hard_help();

    /** @brief Outputs the current version of the tool. */
    void version();

    /** @brief Lists all available rules defined in the context. */
    void list_rules();

    /** @brief Lists all defined or active variables. */
    void list_variables();

public:
    /**
     * @brief Constructs a new `Metaprocessor` with the given execution context.
     * @param context A unique pointer to the current execution context.
     */
    Metaprocessor(std::unique_ptr<Context> context);

    /**
     * @brief Executes the appropriate meta-command based on the context.
     *
     * This method overrides `Processor::process()` and dispatches to the
     * correct handler (e.g. `help()`, `version()`, `list_rules()`).
     */
    void process() override;
};

}
