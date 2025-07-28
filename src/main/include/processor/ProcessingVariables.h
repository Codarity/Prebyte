#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <random>
#include <format>
#include <functional>

#include "datatypes/Context.h"

extern const std::string VERSION;

namespace prebyte {



/**
 * @brief Enumeration of supported built-in variable actions.
 *
 * `VariableAction` defines the list of system-level or context-aware variables
 * that can be dynamically resolved during processing (e.g., timestamps, user info, file metadata).
 */
enum class VariableAction {
    DATE,           /**< Current date (e.g., YYYY-MM-DD). */
    TIME,           /**< Current time (e.g., HH:MM:SS). */
    DATETIME,       /**< Current date and time. */
    LINE,           /**< Current line number (if tracked). */
    VERSION,        /**< Application version. */
    YEAR,           /**< Current year (e.g., 2025). */
    MONTH,          /**< Current month (e.g., 07). */
    DAY,            /**< Current day of the month. */
    HOUR,           /**< Current hour. */
    MINUTE,         /**< Current minute. */
    SECOND,         /**< Current second. */
    UNIXTIMESTAMP,  /**< Current time in Unix timestamp format. */
    USER,           /**< Username of the current system user. */
    HOST,           /**< Hostname of the system. */
    PWD,            /**< Current working directory. */
    FILE,           /**< Full path to the currently processed file. */
    FILE_NAME,      /**< File name without path. */
    FILE_PATH,      /**< Path portion of the current file. */
    FILE_EXT,       /**< File extension. */
    FILE_SIZE,      /**< File size in bytes (as string). */
    FILE_CREATED    /**< File creation timestamp (if available). */
};

/**
 * @brief Provides dynamic resolution for built-in system or context variables.
 *
 * The `ProcessingVariables` class maps internal keywords (like `${DATE}`, `${USER}`, `${FILE_SIZE}`) to
 * real-time system values or context-dependent data. It allows consistent access to these
 * values during macro or template processing.
 *
 * This class is constructed with a pointer to the current `Context`, and uses standard
 * library functions and OS interfaces to provide accurate and timely values.
 */
class ProcessingVariables {
private:
    std::unordered_map<std::string, std::function<std::string()>> functionality;  ///< Mapping of variable keys to resolver functions.
    std::chrono::time_point<std::chrono::system_clock> current_time;              ///< Captured time for consistent timestamp variables.
    Context* context;                                                             ///< Pointer to the current execution context.

    /** @brief Gets the current local time as a `std::tm` structure. */
    std::tm getLocalTime() const;

    /** @brief Returns the current date (YYYY-MM-DD). */
    std::string _DATE() const;

    /** @brief Returns the current time (HH:MM:SS). */
    std::string _TIME() const;

    /** @brief Returns the current line number (if supported). */
    std::string _LINE() const;

    /** @brief Returns the current date and time (formatted). */
    std::string _DATETIME() const;

    /** @brief Returns the application version. */
    std::string _VERSION() const;

    /** @brief Returns the current year. */
    std::string _YEAR() const;

    /** @brief Returns the current month. */
    std::string _MONTH() const;

    /** @brief Returns the current day of the month. */
    std::string _DAY() const;

    /** @brief Returns the current hour. */
    std::string _HOUR() const;

    /** @brief Returns the current minute. */
    std::string _MINUTE() const;

    /** @brief Returns the current second. */
    std::string _SECOND() const;

    /** @brief Returns the current time as Unix timestamp. */
    std::string _UNIXTIMESTAMP() const;

    /** @brief Returns the username of the current system user. */
    std::string _USER() const;

    /** @brief Returns the hostname of the current machine. */
    std::string _HOST() const;

    /** @brief Returns the current working directory. */
    std::string _PWD() const;

    /** @brief Returns the full path of the currently processed file. */
    std::string _FILE() const;

    /** @brief Returns the file name of the current file. */
    std::string _FILE_NAME() const;

    /** @brief Returns the path portion of the current file. */
    std::string _FILE_PATH() const;

    /** @brief Returns the file extension. */
    std::string _FILE_EXT() const;

    /** @brief Returns the size of the current file in bytes. */
    std::string _FILE_SIZE() const;

    /** @brief Returns the creation time of the current file. */
    std::string _FILE_CREATED() const;

public:
    /**
     * @brief Constructs the processing variable system.
     * @param context Pointer to the execution context for file and environment data.
     */
    ProcessingVariables(Context* context);

    /**
     * @brief Checks if the given key is a valid variable name.
     * @param key Name of the variable (e.g., "DATE", "USER").
     * @return `true` if the variable is supported; otherwise `false`.
     */
    bool is_valid(const std::string& key) const;

    /**
     * @brief Resolves the value of a supported variable.
     * @param key Variable name to resolve (e.g., "YEAR").
     * @return The resolved value as a string.
     * @throws std::runtime_error if the variable is unknown.
     */
    std::string get_value(const std::string& key) const;
};

}
