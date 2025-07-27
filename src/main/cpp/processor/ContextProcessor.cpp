#include "processor/ContextProcessor.h"

namespace prebyte {

ContextProcessor::ContextProcessor(const CliStruct& cli_struct, std::unique_ptr<Context> context)
    : cli_struct(cli_struct), context(std::move(context))  {
}

std::unique_ptr<Context> ContextProcessor::process() {
        this->set_logger();
        this->context->rules.init();
        this->context->start_time = cli_struct.start_time;
        load_action_type();
        load_settings();
        load_profiles();
        load_variables();
        load_ignore();
        load_rules();
        return std::move(context);
}

void ContextProcessor::load_action_type() {
        this->context->logger->debug("Loading action type from CLI struct");
        context->action_type = cli_struct.action;
        context->inputs = cli_struct.input_args;
        this->context->logger->debug("Action type set to: {}", static_cast<int>(context->action_type));
}

std::optional<std::filesystem::path> ContextProcessor::find_settings_file(const std::filesystem::path& dir) const {
    this->context->logger->info("Searching for settings file in directory: {}", dir.string());
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) return std::nullopt;

    const std::string target_stem = "settings";
    const std::vector<std::string> valid_extensions = {
        ".json", ".yaml", ".yml", ".toml"
    };

    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        const std::filesystem::path& path = entry.path();
        this->context->logger->trace("Checking file as potential settings file: {}", path.string());
        if (path.stem() == target_stem) {
            this->context->logger->trace("File stem matches target: {}", path.stem().string());
            std::string ext = path.extension().string();
            this->context->logger->trace("File extension: {}", ext);
            if (std::find(valid_extensions.begin(), valid_extensions.end(), ext) != valid_extensions.end()) {
                this->context->logger->debug("Found settings file: {}", path.string());
                return path;
            }
        }
    }
    this->context->logger->debug("No settings file found in directory: {}", dir.string());
    return std::nullopt;
}

void ContextProcessor::load_settings() {
        this->context->logger->debug("Starting to load settings");
        std::filesystem::path settings_path;
        if (!cli_struct.settings_file.empty()) {
                this->context->logger->debug("Using settings file from argument: {}", cli_struct.settings_file);
                settings_path = cli_struct.settings_file;
        } else {
                std::string path_str = getenv("HOME") + std::string("/.prebyte");
                auto std_settings_path = find_settings_file(path_str);
                if (!std_settings_path) {
                        this->context->logger->warn("No settings file found");
                        return;
                }
                settings_path = *std_settings_path;
                this->context->logger->debug("Using default settings file: {}", settings_path.string());
        }
        FileParser file_parser;
        Data settings_data = file_parser.parse(settings_path);
        this->context->logger->info("Processing settings file: {}", settings_path.string());
        if (settings_data.is_map()) {
                Data variables = settings_data["variables"];
                Data profiles  = settings_data["profiles"];
                Data ignore    = settings_data["ignore"];
                Data rules     = settings_data["rules"];
                if (!variables.is_null()) {
                        this->context->logger->debug("Loading variables from settings file");
                        context->variables = get_variables(variables);
                }
                if (!profiles.is_null()) {
                        this->context->logger->debug("Loading profiles from settings file");
                        context->profiles = get_profiles(profiles);
                }
                if (!ignore.is_null()) {
                        this->context->logger->debug("Loading ignore items from settings file");
                        context->ignore = get_ignore(ignore);
                }
                if (!rules.is_null()) {
                        this->context->logger->debug("Loading rules from settings file");
                        load_rules(get_rules(rules));
                }
        }
}

void ContextProcessor::load_profiles() {
        this->context->logger->info("Loading active profiles from CLI");
        if (cli_struct.profiles.empty()) {
                this->context->logger->debug("No profiles specified in CLI");
                return;
        }
        for(const std::string profile_name : cli_struct.profiles) {
                this->context->logger->debug("Loading profile: {}", profile_name);
                Profile profile = context->profiles[profile_name];
                this->context->logger->debug("Profile '{}' found with {} variables, {} ignore items, and {} rules",
                                      profile_name, profile.get_variables().size(),
                                      profile.get_ignore().size(), profile.get_rules().size());
                this->context->logger->debug("Adding variables from active profile: '{}'", profile_name);
                for (const auto& [key,value] : profile.get_variables()) {
                        this->context->logger->trace("Profile: '{}' variable: '{}'", profile_name, key);
                        context->variables[key] = {value};
                }
                this->context->logger->debug("Adding ignore variables from active profile: '{}'", profile_name);
                for (const auto& ignore_item : profile.get_ignore()) {
                        this->context->logger->trace("Profile: '{}' ignore item: '{}'", profile_name, ignore_item);
                        context->ignore.insert(ignore_item);
                }
                this->context->logger->debug("Adding rules from active profile: '{}'", profile_name);
                for (const auto& [rule_name, rule_value] : profile.get_rules()) {
                        this->context->logger->trace("Profile {} adding rule {}", 
                                      profile_name, rule_name);
                        context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_value)));
                }
        }
}

void ContextProcessor::load_variables() {
        this->context->logger->info("Adding variables from CLI");
        if (cli_struct.variables.empty()) {
                this->context->logger->debug("No variables specified in CLI");
                return;
        }
        this->context->logger->debug("Processing variables from CLI");
        for (const auto& variable : cli_struct.variables) {
                size_t pos = variable.find('=');
                if (pos == std::string::npos) {
                        this->context->logger->debug("Variable '{}' does not contain '='. Attempting to inject as file path.", variable);
                        inject_variables(variable);
                        continue;
                }

                std::string var_name = variable.substr(0, pos);
                std::string var_value = variable.substr(pos + 1);
                this->context->logger->trace("Found variable: {} with value: {}", var_name, var_value);
                if (var_name.empty()) {
                        this->context->logger->error("Variable name cannot be empty.");
                        end(this->context.get());
                } else if (var_value.empty()) {
                        this->context->logger->error("Variable value cannot be empty for variable: {}", var_name);
                        end(this->context.get());
                }

                if (var_value[0] == '[' && var_value.back() == ']') {
                        this->context->logger->debug("Variable '{}' is an array: {}", var_name, var_value);
                        var_value = var_value.substr(1, var_value.size() - 2);
                        std::vector<std::string> values;
                        std::istringstream ss(var_value);
                        std::string item;
                        while (std::getline(ss, item, ',')) {
                                if (!item.empty()) {
                                        this->context->logger->trace("Adding item '{}' to array variable '{}'", item, var_name);
                                        values.push_back(item);
                                }
                        }
                        context->variables[var_name] = values;
                        if (context->logger->should_log(spdlog::level::trace)) {
                                std::string value_str = "Variable " + var_name +" added with values: [";
                                for (const auto& val : values) {
                                        value_str += "'" + val + "', ";
                                }
                                value_str = value_str.substr(0, value_str.size() - 2) + "]";
                                this->context->logger->trace(value_str);
                        }
                        continue;
                }
                this->context->logger->debug("Adding variable '{}' with value '{}'", var_name, var_value);
                context->variables[var_name] = {var_value};
        }
}

void ContextProcessor::inject_variables(const std::string& filePath) {
        this->context->logger->debug("Injecting variables from file: {}", filePath);
        if (!std::filesystem::exists(filePath)) {
                this->context->logger->error("File does not exist: {}", filePath);
                end(this->context.get());
        }
        FileParser file_parser;
        Data file_data = file_parser.parse(filePath);
        if (!file_data.is_map()) {
                this->context->logger->error("File content is not a map: {}. Make sure it is a valid JSON, YAML or TOML file.", filePath);
                end(this->context.get());
        }
        for (const auto& [key, value] : file_data.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        this->context->logger->error("Variable name cannot be empty in file: {}", filePath);
                        end(this->context.get());
                }
                if (value.is_null()) {
                        this->context->logger->error("Variable value cannot be null for variable: {} in file: {}", variable_name, filePath);
                        end(this->context.get());
                }
                this->context->logger->debug("Injecting variable {}", variable_name);
                context->variables[variable_name] = {value.as_string()};
        }
}

void ContextProcessor::load_ignore() {
        this->context->logger->info("Adding ignore items");
        if (cli_struct.ignore.empty()) {
                this->context->logger->debug("No ignore items specified in CLI");
                return;
        }
        for (const auto& ignore_item : cli_struct.ignore) {
                if (ignore_item.empty()) {
                        this->context->logger->error("Ignore item cannot be empty.");
                        end(this->context.get());
                }
                this->context->logger->debug("Adding ignore item: {}", ignore_item);
                context->ignore.insert(ignore_item);
        }
}

void ContextProcessor::load_rules() {
        this->context->logger->info("Adding rules from CLI");
        if (cli_struct.rules.empty()) {
                this->context->logger->debug("No rules specified in CLI");
                return;
        }
        for (const auto& rule : cli_struct.rules) {
                size_t pos = rule.find('=');
                if (pos == std::string::npos) {
                        this->context->logger->error("Rule '{}' does not contain '='. Rules must be in the format 'name=value'.", rule);
                        end(this->context.get());
                }
                std::string rule_name = rule.substr(0, pos);
                std::string rule_value = rule.substr(pos + 1);
                if (rule_name.empty() || rule_value.empty()) {
                        this->context->logger->error("Rule name or value cannot be empty. Rule: '{}'", rule);
                        end(this->context.get());
                }
                this->context->logger->trace("Found rule: '{}' with value: '{}'", rule_name, rule_value);
                context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_value)));
        }
}

void ContextProcessor::load_rules(const std::map<std::string, std::string>& rules) {
        this->context->logger->info("Adding rules from settings file");
        for (const auto& [rule_name, rule_data] : rules) {
                this->context->logger->trace("Found rule: '{}' with value: '{}'", rule_name, rule_data);
                context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_data)));
        }
}

std::map<std::string,std::vector<std::string>> ContextProcessor::get_variables(const Data& variables) {
        std::map<std::string,std::vector<std::string>> variable_list;
        this->context->logger->debug("Processing variables from Data object");
        for (const auto& [key, value] : variables.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        this->context->logger->error("Variable name cannot be empty.");
                        end(this->context.get());
                }
                if (value.is_null()) {
                        this->context->logger->error("Variable value cannot be null for variable: {}", variable_name);
                        end(this->context.get());
                }
                if (value.is_string()) {
                        this->context->logger->trace("Variable: '{}' is a string with value: '{}'", variable_name, value.as_string());
                        variable_list[variable_name] = {value.as_string()};
                } else if (value.is_int()) {
                        this->context->logger->trace("Variable: '{}' is an integer with value: {}", variable_name, value.as_int());
                        variable_list[variable_name] = {std::to_string(value.as_int())};
                } else if (value.is_double()) {
                        this->context->logger->trace("Variable: '{}' is a double with value: {}", variable_name, value.as_double());
                        variable_list[variable_name] = {std::to_string(value.as_double())};
                } else if (value.is_bool()) {
                        this->context->logger->trace("Variable: '{}' is a boolean with value: {}", variable_name, value.as_bool());
                        variable_list[variable_name] = {value.as_bool() ? "true" : "false"};
                } else if (value.is_array()) {
                        std::vector<std::string> values;
                        this->context->logger->trace("Variable: '{}' is an array", variable_name);
                        for (const auto& item : value.as_array()) {
                                if (item.is_null() || item.is_array() || item.is_map()) {
                                        this->context->logger->error("Variable value cannot be null or an array/map for variable: {}", variable_name);
                                        end(this->context.get());
                                }
                                if (item.is_string()) {
                                        this->context->logger->trace("Array item for variable: '{}' is a string with value: '{}'", variable_name, item.as_string());
                                        variable_list[variable_name] = {item.as_string()};
                                } else if (item.is_int()) {
                                        this->context->logger->trace("Array item for variable: '{}' is an integer with value: {}", variable_name, item.as_int());
                                        variable_list[variable_name] = {std::to_string(item.as_int())};
                                } else if (item.is_double()) {
                                        this->context->logger->trace("Array item for variable: '{}' is a double with value: {}", variable_name, item.as_double());
                                        variable_list[variable_name] = {std::to_string(item.as_double())};
                                } else if (item.is_bool()) {
                                        this->context->logger->trace("Array item for variable: '{}' is a boolean with value: {}", variable_name, item.as_bool());
                                        variable_list[variable_name] = {item.as_bool() ? "true" : "false"};
                                }
                                values.push_back(item.as_string());
                        }
                        if (this->context->logger->should_log(spdlog::level::trace)) {
                                std::string value_str = "Variable " + variable_name + " added with values: [";
                                for (const auto& val : values) {
                                        value_str += "'" + val + "', ";
                                }
                                value_str = value_str.substr(0, value_str.size() - 2) + "]";
                                this->context->logger->trace(value_str);
                        }
                        variable_list[variable_name] = values;
                } else {
                        this->context->logger->error("Unsupported variable type for variable: {}", variable_name);
                        end(this->context.get());
                }
        }
        return variable_list;
}

std::map<std::string,Profile> ContextProcessor::get_profiles(const Data& profiles) {
        std::map<std::string,Profile> profile_list;
        this->context->logger->debug("Processing profiles from Data object");
        for (const auto& [key, profile_value] : profiles.as_map()) {
                std::string profile_name = key;
                if (profile_name.empty()) {
                        this->context->logger->error("Profile name cannot be empty.");
                        end(this->context.get());
                }
                if (!profile_value.is_map()) {
                        this->context->logger->error("Profile value must be a map for profile: {}", profile_name);
                        end(this->context.get());
                }
                this->context->logger->trace("Creating profile: {}", profile_name);
                Profile profile(profile_name);
                for (const auto& [var_key, var_value] : profile_value.as_map()) {
                        if (var_key == "variables") {
                                this->context->logger->trace("Adding variables to profile: {}", profile_name);
                                profile.add_variable(get_variables(var_value));
                        } else if (var_key == "ignore") {
                                this->context->logger->trace("Adding ignore items to profile: {}", profile_name);
                                profile.add_ignore(get_ignore(var_value));
                        } else if (var_key == "rules") {
                                this->context->logger->trace("Adding rules to profile: {}", profile_name);
                                profile.add_rules(get_rules(var_value));
                        } else {
                                this->context->logger->error("Unknown key '{}' in profile: {}", var_key, profile_name);
                                end(this->context.get());
                        }
                }
                this->context->logger->trace("Profile '{}' created with {} variables, {} ignore items, and {} rules",
                                      profile_name, profile.get_variables().size(),
                                      profile.get_ignore().size(), profile.get_rules().size());
                profile_list[profile_name] = profile;
        }
        return profile_list;
}

std::unordered_set<std::string> ContextProcessor::get_ignore(const Data& ignore) {
        std::unordered_set<std::string> ignore_list;
        this->context->logger->debug("Processing ignore items from Data object");
        if (!ignore.is_array()) {
                this->context->logger->error("Ignore must be an array.");
                end(this->context.get());
        }
        for (const auto& item : ignore.as_array()) {
                if (!item.is_string()) {
                        this->context->logger->error("Ignore items must be strings.");
                        end(this->context.get());
                }
                this->context->logger->trace("Adding ignore item: '{}'", item.as_string());
                ignore_list.insert(item.as_string());
        }
        return ignore_list;
}

std::map<std::string,std::string> ContextProcessor::get_rules(const Data& rules) {
        std::map<std::string,std::string> rule_set;
        this->context->logger->debug("Processing rules from Data object");
        if (!rules.is_map()) {
                this->context->logger->error("Rules must be a map.");
                end(this->context.get());
        }
        for (const auto& [key, value] : rules.as_map()) {
                std::string rule_name = key;
                if (rule_name.empty()) {
                        this->context->logger->error("Rule name cannot be empty.");
                        end(this->context.get());
                }
                if (!value.is_string() && !value.is_bool() && !value.is_int() && !value.is_double()) {
                        this->context->logger->error("Rule value must be a string, boolean, integer, or double for rule: {}", rule_name);
                        end(this->context.get());
                }
                this->context->logger->trace("Adding rule: '{}' with value: '{}'", rule_name, value.as_string());
                rule_set[rule_name] = value.as_string();
        }
        this->context->logger->debug("Processed {} rules", rule_set.size());
        return rule_set;
}


void ContextProcessor::set_logger() {
        if (this->context->logger) {
                this->context->logger->debug("Logger already set, skipping initialization.");
                return;
        }
        pid_t pid = getpid();
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(expand_tilde("~/.prebyte/prebyte.log"), false);
        file_sink->set_level(spdlog::level::debug);
        std::string file_pattern = "%Y-%m-%d %H:%M:%S.%e {" + std::to_string(pid) + "} [%l] %v";
        file_sink->set_pattern(file_pattern);

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::err);
        console_sink->set_pattern("%^[%l] %v%$");

        context->console_sink = console_sink;

        context->logger = std::make_shared<spdlog::logger>("prebyte", spdlog::sinks_init_list{file_sink, console_sink});

        context->logger->debug("Logger initialized with PID: {}", pid);
}

std::string ContextProcessor::expand_tilde(const std::string& path) {
    if (path.starts_with("~/")) {
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + path.substr(1);
        }
    }
    return path;
}


} // namespace prebyte
