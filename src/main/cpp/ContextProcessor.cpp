#include "ContextProcessor.h"

namespace prebyte {

ContextProcessor::ContextProcessor(const CliStruct& cli_struct)
    : cli_struct(cli_struct) {
}

Context ContextProcessor::process() {
        load_action_type();
        load_settings();
        load_profiles();
        load_variables();
        load_ignore();
        load_rules();
        return this->context;
}

void ContextProcessor::load_action_type() {
        context.action_type = cli_struct.action;
        context.inputs = cli_struct.input_args;
}

std::optional<std::filesystem::path> ContextProcessor::find_settings_file(const std::filesystem::path& dir) const {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) return std::nullopt;

    const std::string target_stem = "settings";
    const std::vector<std::string> valid_extensions = {
        ".json", ".yaml", ".yml", ".xml", ".toml"
    };

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        const std::filesystem::path& path = entry.path();
        if (path.stem() == target_stem) {
            std::string ext = path.extension().string();
            if (std::find(valid_extensions.begin(), valid_extensions.end(), ext) != valid_extensions.end()) {
                return path;
            }
        }
    }

    return std::nullopt;
}

void ContextProcessor::load_settings() {
        std::string path_str = getenv("HOME") + std::string("/.prebyte");
        auto settings_path = find_settings_file(path_str);
        if (!settings_path) {
                return;
        }
        FileParser file_parser;
        Data settings_data = file_parser.parse(*settings_path);
        if (settings_data.is_map()) {
                Data variables = settings_data["variables"];
                Data profiles = settings_data["profiles"];
                Data ignore = settings_data["ignore"];
                Data rules = settings_data["rules"];
                if (!variables.is_null()) {
                        context.variables = get_variables(variables);
                }
                if (!profiles.is_null()) {
                        context.profiles = get_profiles(profiles);
                }
                if (!ignore.is_null()) {
                        context.ignore = get_ignore(ignore);
                }
                if (!rules.is_null()) {
                        load_rules(get_rules(rules));
                }
        }
}

void ContextProcessor::load_profiles() {
        if (cli_struct.profiles.empty()) {
                return;
        }
        for(const std::string profile_name : cli_struct.profiles) {
                Profile profile = context.profiles[profile_name];
                for (const auto& [key,value] : profile.get_variables()) {
                        context.variables[key] = value;
                }
                for (const auto& ignore_item : profile.get_ignore()) {
                        context.ignore.insert(ignore_item);
                }
                for (const auto& [rule_name, rule_value] : profile.get_rules()) {
                        this->add_rule(context.rules, rule_name, Data(rule_value));
                }
        }
}

void ContextProcessor::load_variables() {
        if (cli_struct.variables.empty()) {
                return;
        }
        for (const auto& variable : cli_struct.variables) {
                size_t pos = variable.find('=');
                if (pos == std::string::npos) {
                        inject_variables(variable);
                        continue;
                }
                std::string var_name = variable.substr(0, pos);
                std::string var_value = variable.substr(pos + 1);
                if (var_name.empty() || var_value.empty()) {
                        throw std::runtime_error("Variable name or value cannot be empty.");
                }
                context.variables[var_name] = var_value;
        }
}

void ContextProcessor::inject_variables(const std::string& filePath) {
        if (!std::filesystem::exists(filePath)) {
                throw std::runtime_error("File does not exist: " + filePath);
        }
        FileParser file_parser;
        Data file_data = file_parser.parse(filePath);
        if (!file_data.is_map()) {
                throw std::runtime_error("File content must be a map.");
        }
        for (const auto& [key, value] : file_data.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        throw std::runtime_error("Variable name cannot be empty.");
                }
                context.variables[variable_name] = value.as_string();
        }
}

void ContextProcessor::load_ignore() {
        if (cli_struct.ignore.empty()) {
                return;
        }
        for (const auto& ignore_item : cli_struct.ignore) {
                if (ignore_item.empty()) {
                        throw std::runtime_error("Ignore item cannot be empty.");
                }
                context.ignore.insert(ignore_item);
        }
}

void ContextProcessor::load_rules() {
        if (cli_struct.rules.empty()) {
                return;
        }
        for (const auto& rule : cli_struct.rules) {
                size_t pos = rule.find('=');
                if (pos == std::string::npos) {
                        throw std::runtime_error("Rule must be in the format 'name=value'.");
                }
                std::string rule_name = rule.substr(0, pos);
                std::string rule_value = rule.substr(pos + 1);
                if (rule_name.empty() || rule_value.empty()) {
                        throw std::runtime_error("Rule name or value cannot be empty.");
                }
                add_rule(context.rules, rule_name, Data(rule_value));
        }
}

void ContextProcessor::load_rules(const std::map<std::string, std::string>& rules) {
        for (const auto& [rule_name, rule_data] : rules) {
                add_rule(context.rules, rule_name, Data(rule_data));
        }
}

std::map<std::string,std::string> ContextProcessor::get_variables(const Data& variables) {
        std::map<std::string,std::string> variable_list;
        for (const auto& [key, value] : variables.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        throw std::runtime_error("Variable name cannot be empty.");
                }
                if (value.is_null()) {
                        throw std::runtime_error("Variable value cannot be null.");
                }
                if (value.is_string()) {
                        variable_list[variable_name] = value.as_string();
                } else if (value.is_int()) {
                        variable_list[variable_name] = std::to_string(value.as_int());
                } else if (value.is_double()) {
                        variable_list[variable_name] = std::to_string(value.as_double());
                } else if (value.is_bool()) {
                        variable_list[variable_name] = value.as_bool() ? "true" : "false";
                } else {
                        throw std::runtime_error("Unsupported variable type for variable: " + variable_name);
                }
        }
        return variable_list;
}

std::map<std::string,Profile> ContextProcessor::get_profiles(const Data& profiles) {
        std::map<std::string,Profile> profile_list;
        for (const auto& [key, profile_value] : profiles.as_map()) {
                std::string profile_name = key;
                if (profile_name.empty()) {
                        throw std::runtime_error("Profile name cannot be empty.");
                }
                if (!profile_value.is_map()) {
                        throw std::runtime_error("Profile value must be a map.");
                }
                Profile profile(profile_name);
                for (const auto& [var_key, var_value] : profile_value.as_map()) {
                        if (var_key == "variables") {
                                profile.add_variable(get_variables(var_value));
                        } else if (var_key == "ignore") {
                                profile.add_ignore(get_ignore(var_value));
                        } else if (var_key == "rules") {
                                profile.add_rules(get_rules(var_value));
                        } else {
                                throw std::runtime_error("Unknown key in profile: " + var_key);
                        }
                }
                profile_list[profile_name] = profile;
        }
        return profile_list;
}

std::unordered_set<std::string> ContextProcessor::get_ignore(const Data& ignore) {
        std::unordered_set<std::string> ignore_list;
        if (!ignore.is_array()) {
                throw std::runtime_error("Ignore must be an array.");
        }
        for (const auto& item : ignore.as_array()) {
                if (!item.is_string()) {
                        throw std::runtime_error("Ignore items must be strings.");
                }
                ignore_list.insert(item.as_string());
        }
        return ignore_list;
}

std::map<std::string,std::string> ContextProcessor::get_rules(const Data& rules) {
        std::map<std::string,std::string> rule_set;
        if (!rules.is_map()) {
                throw std::runtime_error("Rules must be a map.");
        }
        for (const auto& [key, value] : rules.as_map()) {
                std::string rule_name = key;
                if (rule_name.empty()) {
                        throw std::runtime_error("Rule name cannot be empty.");
                }
                if (!value.is_string() && !value.is_bool() && !value.is_int() && !value.is_double()) {
                        throw std::runtime_error("Rule value must be a string, boolean, integer, or double.");
                }
                rule_set[rule_name] = value.as_string(); // Convert all values to string for consistency
        }
        return rule_set;
}

void ContextProcessor::add_rule(Rules& rules, std::string rule_name, const Data& rule_data) {
        if (rule_name == "strict_variables") {
                rules.strict_variables = get_bool(rule_data.as_bool());
        } else if (rule_name == "set_default_variables") {
                rules.set_default_variables = get_bool(rule_data.as_bool());
        } else if (rule_name == "trim_start") {
                rules.trim_start = get_bool(rule_data.as_bool());
        } else if (rule_name == "trim_end") {
                rules.trim_end = get_bool(rule_data.as_bool());
        } else if (rule_name == "allow_env") {
                rules.allow_env = get_bool(rule_data.as_bool());
        } else if (rule_name == "debug_level") {
                std::string debug_level_str = get_string(rule_data);
                if (debug_level_str == "ERROR") {
                        rules.debug_level = DebugLevel::ERROR;
                } else if (debug_level_str == "WARNING") {
                        rules.debug_level = DebugLevel::WARNING;
                } else if (debug_level_str == "INFO") {
                        rules.debug_level = DebugLevel::INFO;
                } else if (debug_level_str == "DEBUG") {
                        rules.debug_level = DebugLevel::DEBUG;
                } else {
                        throw std::runtime_error("Unknown debug level: " + debug_level_str);
                }
        } else if (rule_name == "max_variable_length") {
                rules.max_variable_length = get_int(rule_data);
        } else if (rule_name == "default_variable_value") {
                rules.default_variable_value = get_string(rule_data);
        } else if (rule_name == "variable_prefix") {
                rules.variable_prefix = get_string(rule_data);
        } else if (rule_name == "variable_suffix") {
                rules.variable_suffix = get_string(rule_data);
        } else if (rule_name == "include_path") {
                rules.include_path = get_string(rule_data);
        } else if (rule_name == "benchmark") {
                std::string benchmark_str = get_string(rule_data);
                if (benchmark_str == "NONE") {
                        rules.benchmark = Benchmark::NONE;
                } else if (benchmark_str == "TIME") {
                        rules.benchmark = Benchmark::TIME;
                } else if (benchmark_str == "MEMORY") {
                        rules.benchmark = Benchmark::MEMORY;
                } else if (benchmark_str == "ALL") {
                        rules.benchmark = Benchmark::ALL;
                } else {
                        throw std::runtime_error("Unknown benchmark type: " + benchmark_str);
                }
        } else {
                throw std::runtime_error("Unknown rule: " + rule_name);
        }
}

bool ContextProcessor::get_bool(Data data) const {
        if (!data.is_bool()) {
                throw std::runtime_error("Expected boolean value.");
        }
        return data.is_bool();
}

std::string ContextProcessor::get_string(Data data) const {
        if (!data.is_string()) {
                throw std::runtime_error("Expected string value.");
        }
        return data.as_string();
}

int ContextProcessor::get_int(Data data) const {
        if (!data.is_int()) {
                throw std::runtime_error("Expected integer value.");
        }
        return data.as_int();
}

double ContextProcessor::get_double(Data data) const {
        if (!data.is_double()) {
                throw std::runtime_error("Expected double value.");
        }
        return data.as_double();
}

} // namespace prebyte
