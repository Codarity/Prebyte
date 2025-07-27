#include "ContextProcessor.h"

namespace prebyte {

ContextProcessor::ContextProcessor(const CliStruct& cli_struct)
    : cli_struct(cli_struct) {
}

Context ContextProcessor::process() {
        this->context.rules.init();
        this->context.start_time = cli_struct.start_time;
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
        std::filesystem::path settings_path;
        if (!cli_struct.settings_file.empty()) {
                settings_path = cli_struct.settings_file;
        } else {
                std::string path_str = getenv("HOME") + std::string("/.prebyte");
                auto std_settings_path = find_settings_file(path_str);
                if (!std_settings_path) {
                        return;
                }
                settings_path = *std_settings_path;
        }
        FileParser file_parser;
        Data settings_data = file_parser.parse(settings_path);
        if (settings_data.is_map()) {
                Data variables = settings_data["variables"];
                Data profiles  = settings_data["profiles"];
                Data ignore    = settings_data["ignore"];
                Data rules     = settings_data["rules"];
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
                        context.variables[key] = {value};
                }
                for (const auto& ignore_item : profile.get_ignore()) {
                        context.ignore.insert(ignore_item);
                }
                for (const auto& [rule_name, rule_value] : profile.get_rules()) {
                        context.rules.add_rule(rule_name, Data(rule_value));
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

                if (var_value[0] == '[' && var_value.back() == ']') {
                        var_value = var_value.substr(1, var_value.size() - 2);
                        std::vector<std::string> values;
                        std::istringstream ss(var_value);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                                if (!item.empty()) {
                                        values.push_back(item);
                                }
                        }
                        context.variables[var_name] = values;
                        continue;
                }
                context.variables[var_name] = {var_value};
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
                context.variables[variable_name] = {value.as_string()};
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
                context.rules.add_rule(rule_name, Data(rule_value));
        }
}

void ContextProcessor::load_rules(const std::map<std::string, std::string>& rules) {
        for (const auto& [rule_name, rule_data] : rules) {
                context.rules.add_rule(rule_name, Data(rule_data));
        }
}

std::map<std::string,std::vector<std::string>> ContextProcessor::get_variables(const Data& variables) {
        std::map<std::string,std::vector<std::string>> variable_list;
        for (const auto& [key, value] : variables.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        throw std::runtime_error("Variable name cannot be empty.");
                }
                if (value.is_null()) {
                        throw std::runtime_error("Variable value cannot be null.");
                }
                if (value.is_string()) {
                        variable_list[variable_name] = {value.as_string()};
                } else if (value.is_int()) {
                        variable_list[variable_name] = {std::to_string(value.as_int())};
                } else if (value.is_double()) {
                        variable_list[variable_name] = {std::to_string(value.as_double())};
                } else if (value.is_bool()) {
                        variable_list[variable_name] = {value.as_bool() ? "true" : "false"};
                } else if (value.is_array()) {
                        std::vector<std::string> values;
                        for (const auto& item : value.as_array()) {
                                if (item.is_null() || item.is_array() || item.is_map()) {
                                        throw std::runtime_error("Variable value cannot be null.");
                                }
                                if (item.is_string()) {
                                        variable_list[variable_name] = {item.as_string()};
                                } else if (item.is_int()) {
                                        variable_list[variable_name] = {std::to_string(item.as_int())};
                                } else if (item.is_double()) {
                                        variable_list[variable_name] = {std::to_string(item.as_double())};
                                } else if (item.is_bool()) {
                                        variable_list[variable_name] = {item.as_bool() ? "true" : "false"};
                                }
                                values.push_back(item.as_string());
                        }
                        variable_list[variable_name] = values;
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
                rule_set[rule_name] = value.as_string();
        }
        return rule_set;
}


} // namespace prebyte
