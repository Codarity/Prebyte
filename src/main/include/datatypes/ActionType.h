#pragma once

namespace prebyte {

/**
 * @brief Enum representing the type of action to be performed.
 *
 * This enum defines all possible actions the program can execute,
 * determining how input is read and how output is written, or triggering
 * special behaviors such as displaying help or version information.
 *
 * Each value corresponds to a different combination of input/output handling
 * or a specific operation. This allows flexible configuration of the program
 * behavior depending on user intent or command-line arguments.
 *
 * ### Action Types:
 * - Input/output modes:
 *   - STDIN_STDOUT: Read from standard input, write to standard output.
 *   - FILE_IN_STDOUT: Read from file, write to standard output.
 *   - STDIN_FILE_OUT: Read from standard input, write to file.
 *   - FILE_IN_FILE_OUT: Read from file, write to file.
 *   - API_IN_API_OUT: Read from internal API input, write to API output.
 *   - API_IN_FILE_OUT: Read from API, write to file.
 *   - FILE_IN_API_OUT: Read from file, write to API.
 *
 * - Special actions:
 *   - NONE: No action specified.
 *   - EXPLAIN: Provide a detailed explanation of the given input.
 *   - HELP: Show usage instructions and available options.
 *   - VERSION: Display program version.
 *   - LIST_RULES: Output a list of available rules.
 *   - LIST_VARIABLES: Output a list of available variables and values.
 *   - HARD_HELP: Show extended or advanced help documentation.
 */
enum class ActionType {
    NONE,               /**< No action specified. Used as a default or placeholder. */
    STDIN_STDOUT,       /**< Read input from stdin and write output to stdout. */
    FILE_IN_STDOUT,     /**< Read input from a file and write to stdout. */
    STDIN_FILE_OUT,     /**< Read input from stdin and write to a file. */
    FILE_IN_FILE_OUT,   /**< Read input from a file and write to another file. */
    EXPLAIN,            /**< Explain the input data in detail, providing insights or transformations. */
    HELP,               /**< Display basic usage and help information. */
    VERSION,            /**< Show version information. */
    LIST_RULES,         /**< List all available rules that the program supports. */
    LIST_VARIABLES,     /**< List all available variables or configuration parameters. */
    API_IN_API_OUT,     /**< Process data via API input and produce API output. */
    API_IN_FILE_OUT,    /**< Read from API and write output to a file. */
    FILE_IN_API_OUT,    /**< Read from a file and produce output to the API. */
    HARD_HELP           /**< Show extended help including detailed examples. */
};

}
