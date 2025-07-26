#pragma once

#include <string>
#include <optional>
#include <filesystem>

#include "datatypes/Data.h"

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
        std::optional<bool> strict_variables;
        std::optional<bool> set_default_variables;
        std::optional<bool> trim_start;
        std::optional<bool> trim_end;
        std::optional<bool> allow_env;
        std::optional<bool> allow_env_fallback;
        std::optional<DebugLevel> debug_level;
        std::optional<int> max_variable_length; // -1 = unlimited
        std::optional<std::string> default_variable_value;
        std::optional<std::string> variable_prefix;
        std::optional<std::string> variable_suffix;
        std::optional<std::string> include_path;

        std::optional<Benchmark> benchmark;
        void add_rule(std::string rule_name, const Data& rule_data);

        std::string get_string(Data data);
        int get_int(Data data);
        double get_double(Data data);
        void init();
};


}
