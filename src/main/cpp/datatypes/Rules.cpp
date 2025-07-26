#include "datatypes/Rules.h"

namespace prebyte {
void Rules::add_rule(std::string rule_name, const Data& rule_data) {
        if (rule_name == "strict_variables") {
                this->strict_variables = rule_data.as_bool();
        } else if (rule_name == "set_default_variables") {
                this->set_default_variables = rule_data.as_bool();
        } else if (rule_name == "trim_start") {
                this->trim_start = rule_data.as_bool();
        } else if (rule_name == "trim_end") {
                this->trim_end = rule_data.as_bool();
        } else if (rule_name == "allow_env") {
                this->allow_env = rule_data.as_bool();
        } else if (rule_name == "allow_env_fallback") {
                this->allow_env_fallback = rule_data.as_bool();
        } else if (rule_name == "debug_level") {
                std::string debug_level_str = get_string(rule_data);
                if (debug_level_str == "ERROR") {
                        this->debug_level = DebugLevel::ERROR;
                } else if (debug_level_str == "WARNING") {
                        this->debug_level = DebugLevel::WARNING;
                } else if (debug_level_str == "INFO") {
                        this->debug_level = DebugLevel::INFO;
                } else if (debug_level_str == "DEBUG") {
                        this->debug_level = DebugLevel::DEBUG;
                } else {
                        throw std::runtime_error("Unknown debug level: " + debug_level_str);
                }
        } else if (rule_name == "max_variable_length") {
                this->max_variable_length = get_int(rule_data);
        } else if (rule_name == "default_variable_value") {
                this->default_variable_value = get_string(rule_data);
        } else if (rule_name == "variable_prefix") {
                this->variable_prefix = get_string(rule_data);
        } else if (rule_name == "variable_suffix") {
                this->variable_suffix = get_string(rule_data);
        } else if (rule_name == "include_path") {
                std::string include_path_str = get_string(rule_data);
                if (include_path_str[0] == '~') {
                        include_path_str = getenv("HOME") + include_path_str.substr(1);
                }
                this->include_path = std::filesystem::path(include_path_str);
        } else if (rule_name == "benchmark") {
                std::string benchmark_str = get_string(rule_data);
                if (benchmark_str == "NONE") {
                        this->benchmark = Benchmark::NONE;
                } else if (benchmark_str == "TIME") {
                        this->benchmark = Benchmark::TIME;
                } else if (benchmark_str == "MEMORY") {
                        this->benchmark = Benchmark::MEMORY;
                } else if (benchmark_str == "ALL") {
                        this->benchmark = Benchmark::ALL;
                } else {
                        throw std::runtime_error("Unknown benchmark type: " + benchmark_str);
                }
        } else {
                throw std::runtime_error("Unknown rule: " + rule_name);
        }
}


std::string Rules::get_string(Data data) {
        if (!data.is_string()) {
                throw std::runtime_error("Expected string value.");
        }
        return data.as_string();
}

int Rules::get_int(Data data) {
        if (!data.is_int()) {
                throw std::runtime_error("Expected integer value.");
        }
        return data.as_int();
}

double Rules::get_double(Data data) {
        if (!data.is_double()) {
                throw std::runtime_error("Expected double value.");
        }
        return data.as_double();
}

void Rules::init() {
        this->strict_variables = false;
        this->set_default_variables = false;
        this->trim_start = false;
        this->trim_end = false;
        this->allow_env = true;
        this->allow_env_fallback = false;
        this->debug_level = DebugLevel::ERROR;
        this->max_variable_length = -1; // -1 means no limit
        this->default_variable_value = "???";
        this->variable_prefix = "%%";
        this->variable_suffix = "%%";
        this->add_rule("include_path", Data(this->include_path.value_or("~/.prebyte/includes")));
        this->benchmark = Benchmark::NONE;
}

}
