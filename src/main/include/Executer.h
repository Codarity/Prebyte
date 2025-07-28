#pragma once

#include <stdexcept>

#include "datatypes/Context.h"
#include "processor/Preprocessor.h"
#include "processor/Metaprocessor.h"

namespace prebyte {

/**
 * @brief Top-level execution controller that dispatches processing based on context.
 *
 * The `Executer` class serves as the entry point for triggering program execution.
 * It receives a fully prepared `Context` object (usually constructed by `ContextProcessor`)
 * and is responsible for selecting and running the appropriate processing logic
 * (e.g. `Metaprocessor` for help/version, `Preprocessor` for file processing, etc.).
 */
class Executer {
private:
    std::unique_ptr<Context> context;  ///< The execution context containing all user-defined input and settings.

public:
    /**
     * @brief Constructs the Executer with a prepared execution context.
     * @param context A unique pointer to the context to be executed.
     */
    Executer(std::unique_ptr<Context> context);

    /**
     * @brief Executes the main processing logic based on the provided context.
     *
     * This method serves as the main application dispatcher. It evaluates the `ActionType`
     * in the context and invokes the appropriate processor (e.g., `Metaprocessor`, `Preprocessor`).
     *
     * @throws std::runtime_error if no valid action is specified or processing fails.
     */
    void execute();
};

}
