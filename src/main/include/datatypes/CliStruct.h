#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "datatypes/ActionType.h"

namespace prebyte {
        
/**
 * @brief Struct representing the parsed command-line state.
 *
 * This structure holds all information extracted from the command-line
 * interface (CLI), configuration files, and internal runtime state
 * relevant for processing a single execution of the application.
 *
 * It includes the selected action, input arguments, profiles, rules, 
 * and other flags used to control the program behavior. It also captures 
 * the start time of execution for potential benchmarking and diagnostic purposes.
 *
 * ### Fields overview:
 * - `action`: The action to perform, based on user input.
 * - `start_time`: Timestamp marking the beginning of execution.
 * - `input_args`: Raw command-line arguments.
 * - `profiles`: Named profile sets to load for execution.
 * - `variables`: User-defined or preset variables passed via CLI.
 * - `rules`: List of active rules to apply during processing.
 * - `ignore`: Rules or checks to skip.
 * - `log_level`: Desired logging verbosity ("ERROR", "WARN", "INFO", etc.).
 * - `settings_file`: Path to a custom settings/config file, if provided.
 */
struct CliStruct {
    ActionType action;                     /**< The action to perform (e.g., HELP, FILE_IN_FILE_OUT, etc.). */
    std::chrono::high_resolution_clock::time_point start_time; /**< Timestamp when execution began. */
    std::vector<std::string> input_args;   /**< Original input arguments passed to the program. */
    std::vector<std::string> profiles;     /**< List of selected profiles to load. */
    std::vector<std::string> variables;    /**< Variables passed via CLI or config. */
    std::vector<std::string> rules;        /**< List of rules to apply. */
    std::vector<std::string> ignore;       /**< Rules or checks to ignore. */
    std::string log_level = "";            /**< Logging level. */
    std::string settings_file;             /**< Optional path to a settings/configuration file. */
};

}
