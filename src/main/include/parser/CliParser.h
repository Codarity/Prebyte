#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <list>
#include <spdlog/spdlog.h>

#include "datatypes/CliStruct.h"

namespace prebyte {

/**
 * @brief Parses command-line arguments into a structured representation.
 *
 * The `CliParser` class is responsible for converting raw command-line arguments
 * into a structured `CliStruct`, which contains all relevant data for controlling
 * the application's behavior (action type, input arguments, flags, profiles, etc.).
 *
 * This class encapsulates the logic for identifying the intended action and 
 * processing the CLI input accordingly.
 */
class CliParser {
private:
    CliStruct cli_struct;  ///< Internal storage for the parsed CLI structure.

    /**
     * @brief Determines the action to perform based on CLI arguments.
     *
     * This method analyzes the provided arguments and determines the
     * corresponding `ActionType` (e.g. HELP, EXPLAIN, FILE_IN_FILE_OUT).
     *
     * @param args Pointer to a list of arguments to analyze.
     * @return The determined action type.
     */
    ActionType findAction(std::list<std::string>* args);

    /**
     * @brief Processes the CLI arguments and fills the `CliStruct`.
     *
     * Interprets flags, values, and special commands, and updates
     * the internal `cli_struct` accordingly.
     *
     * @param args A vector of raw CLI arguments (typically from main()).
     */
    void process(const std::vector<std::string>& args);

public:
    /**
     * @brief Parses CLI arguments into a `CliStruct` object.
     *
     * This is the main entry point for parsing user input.
     * It converts and interprets the argument list, identifying the
     * selected action, configuration values, and other parameters.
     *
     * @param args A list of raw string arguments (typically from `argv`).
     * @return A populated `CliStruct` object containing parsed values.
     */
    CliStruct parse(std::list<std::string> args);
};

}
