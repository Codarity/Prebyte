#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "datatypes/Rules.h"
#include "datatypes/ActionType.h"
#include "datatypes/Profile.h"

namespace prebyte {


/**
 * @brief Represents the current execution context of the program.
 *
 * This structure encapsulates all runtime state and configuration 
 * relevant for a single execution of the application, after CLI parsing 
 * and initial setup.
 *
 * It includes information about the selected action, input/output sources, 
 * logging configuration, runtime variables, rules, and active profiles.
 * 
 * The context is passed through various components of the system to maintain 
 * consistent state and logging during processing.
 *
 * ### Fields overview:
 * - `action_type`: The type of action being executed (e.g., HELP, EXPLAIN, API_IN_API_OUT).
 * - `is_api`: Flag indicating whether the execution is API-driven (true) or CLI-driven (false).
 * - `logger`: Shared pointer to the main logger instance.
 * - `console_sink`: Sink used for colored console output.
 * - `rules`: All rules currently loaded and active.
 * - `input`: Primary input data (e.g., raw text or content from a file or API).
 * - `output`: Final output to be written or returned.
 * - `start_time`: Timestamp of execution start, for measuring duration.
 * - `variables`: Map of variable names to their corresponding values.
 * - `inputs`: List of individual input items or sources.
 * - `ignore`: Set of rule names to skip during processing.
 * - `profiles`: Loaded profiles mapped by their names.
 * - `macros`: Macro definitions used for rule preprocessing or template expansion.
 * - `include_counter`: Counter used to detect excessive include recursion or nesting.
 */
struct Context {
    ActionType action_type;  /**< The selected action type (e.g., HELP, FILE_IN_FILE_OUT). */
    bool is_api = false;     /**< Indicates if the program is being run as an API call. */
    std::shared_ptr<spdlog::logger> logger; /**< Shared logger instance for writing logs. */
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink; /**< Sink for colored console output. */
    Rules rules;             /**< Rules currently loaded and used during evaluation. */
    std::string input;       /**< Primary input data (could be file contents or direct string). */
    std::string output;      /**< Resulting output after processing. */
    std::chrono::high_resolution_clock::time_point start_time; /**< Start timestamp of execution. */
    std::map<std::string, std::vector<std::string>> variables; /**< Map of variable names to values. */
    std::vector<std::string> inputs; /**< Individual input strings or sources. */
    std::unordered_set<std::string> ignore; /**< Set of rule names to ignore. */
    std::map<std::string, Profile> profiles; /**< Loaded profiles mapped by name. */
    std::map<std::string, std::string> macros; /**< Macro definitions used for templating or expansion. */
    int include_counter = 0; /**< Tracks include depth or prevent infinite recursion. */
};

/**
 * @brief Cleanly terminates execution by performing any necessary teardown using the context.
 *
 * This function is called at the end of the program, or when an error occurs.
 * If Prebyte is an CLI instance, it will exit with a status code of 1.
 * If it is an API instance, it will throw an runtime_exception.
 *
 * @param context Pointer to the current execution context.
 */
void end(Context* context);

}
