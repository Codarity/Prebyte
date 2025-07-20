#pragma once

#include <string>

namespace prebyte {

enum class Benchmark {
        NONE,
        TIME,
        MEMORY,
        ALL
};

enum class DebugLevel {
        ERROR,
        WARNING,
        INFO,
        DEBUG
};

struct Rules {
        bool strict_variables = false;
        bool set_default_variables = false;
        bool trim_start = false;
        bool trim_end = false;
        bool allow_env = true;
        DebugLevel debug_level = DebugLevel::ERROR;

        int max_variable_length = -1; // -1 means no limit
                                      //
        std::string default_variable_value = "???";
        std::string variable_prefix = "%%";
        std::string variable_suffix = "%%";
        std::string include_path = "~/.prebyte/includes";

        Benchmark benchmark = Benchmark::NONE;
};

}
