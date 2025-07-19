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
        bool case_sensitive_variables = true;
        bool replace_tabs = false;
        bool replace_newlines = false;
        bool trim_start = false;
        bool trim_end = false;
        bool strip_start = false;
        bool strip_end = false;
        bool trim_spaces = false;
        bool allow_includes = true;
        bool allow_env = true;
        bool error_on_false_input = false;
        DebugLevel debug_level = DebugLevel::ERROR;

        int max_variable_length = -1; // -1 means no limit
                                      //
        std::string default_variable_value = "???";
        std::string variable_prefix = "%%";
        std::string variable_suffix = "%%";
        std::string include_path = "~/.prebyte/includes";
        std::string output_encoding = "UTF-8";
        std::string newline_replacement = " ";

        unsigned short tab_size = 4;

        Benchmark benchmark = Benchmark::NONE;
};

}
