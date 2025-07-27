#include "PrebyteEngine.h"


#include "datatypes/Context.h"

#include "parser/FileParser.h"
#include "processor/Preprocessor.h"

namespace prebyte {

std::unique_ptr<Context> context;


std::optional<std::filesystem::path> find_settings_file(const std::filesystem::path& dir);
std::map<std::string,std::string> get_rules(const Data& rules);
std::unordered_set<std::string> get_ignore(const Data& ignore);
std::map<std::string,Profile> get_profiles(const Data& profiles);
std::map<std::string,std::vector<std::string>> get_variables(const Data& variables);
void load_rules(const std::map<std::string, std::string>& rules);


Prebyte::Prebyte() : Prebyte("") {}

Prebyte::Prebyte(std::string settings_file) {
        context = std::make_unique<prebyte::Context>();
        set_logger();
        context->logger->info("Prebyte Engine initialized");
        context->rules.init();
        context->start_time = std::chrono::high_resolution_clock::now();

        std::filesystem::path settings_path;

        context->logger->info("Loading settings file");

        if (!settings_file.empty()) {
                context->logger->debug("Using settings file from argument: {}", settings_file);
                settings_path = settings_file;
        } else {
                context->logger->debug("Using default settings file from HOME directory");
                std::string path_str = getenv("HOME") + std::string("/.prebyte");
                auto std_settings_path = find_settings_file(path_str);
                if (!std_settings_path) {
                        context->logger->warn("No settings file found in HOME directory");
                        return;
                }
                context->logger->debug("Found settings file: {}", std_settings_path->string());
                settings_path = *std_settings_path;
        }

        if (std::filesystem::exists(settings_path)) {
                FileParser file_parser;
                Data settings_data;

                try {
                        settings_data = file_parser.parse(settings_path);
                } catch (const std::exception& e) {
                        context->logger->error("Error parsing settings file: {}", e.what());
                        context->logger->error("Please check the file format and content.");
                        return;
                }

                if (settings_data.is_map()) {
                        Data variables = settings_data["variables"];
                        Data profiles  = settings_data["profiles"];
                        Data ignore    = settings_data["ignore"];
                        Data rules     = settings_data["rules"];
                        if (!variables.is_null()) {
                                context->logger->debug("Loading variables from settings file");
                                context->variables = get_variables(variables);
                        }
                        if (!profiles.is_null()) {
                                context->logger->debug("Loading profiles from settings file");
                                context->profiles = get_profiles(profiles);
                        }
                        if (!ignore.is_null()) {
                                context->logger->debug("Loading ignore items from settings file");
                                context->ignore = get_ignore(ignore);
                        }
                        if (!rules.is_null()) {
                                context->logger->debug("Loading rules from settings file");
                                load_rules(get_rules(rules));
                        }
                }
        }
}



void load_rules(const std::map<std::string, std::string>& rules) {
        for (const auto& [rule_name, rule_data] : rules) {
                context->logger->trace("Found rule: '{}' with value: '{}'", rule_name, rule_data);
                context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_data)));
        }
}


std::map<std::string,std::vector<std::string>> get_variables(const Data& variables) {
        std::map<std::string,std::vector<std::string>> variable_list;
        context->logger->debug("Processing variables from Data object");
        for (const auto& [key, value] : variables.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        context->logger->error("Variable name cannot be empty.");
                        exit(1);
                }
                if (value.is_null()) {
                        context->logger->error("Variable value cannot be null for variable: {}", variable_name);
                        exit(1);
                }
                if (value.is_string()) {
                        context->logger->trace("Variable: '{}' is a string with value: '{}'", variable_name, value.as_string());
                        variable_list[variable_name] = {value.as_string()};
                } else if (value.is_int()) {
                        context->logger->trace("Variable: '{}' is an integer with value: {}", variable_name, value.as_int());
                        variable_list[variable_name] = {std::to_string(value.as_int())};
                } else if (value.is_double()) {
                        context->logger->trace("Variable: '{}' is a double with value: {}", variable_name, value.as_double());
                        variable_list[variable_name] = {std::to_string(value.as_double())};
                } else if (value.is_bool()) {
                        context->logger->trace("Variable: '{}' is a boolean with value: {}", variable_name, value.as_bool());
                        variable_list[variable_name] = {value.as_bool() ? "true" : "false"};
                } else if (value.is_array()) {
                        context->logger->trace("Variable: '{}' is an array", variable_name);
                        std::vector<std::string> values;
                        for (const auto& item : value.as_array()) {
                                if (item.is_null() || item.is_array() || item.is_map()) {
                                        context->logger->error("Variable value cannot be null or an array/map for variable: {}", variable_name);
                                        exit(1);
                                }
                                if (item.is_string()) {
                                        context->logger->trace("Array item for variable: '{}' is a string with value: '{}'", variable_name, item.as_string());
                                        variable_list[variable_name] = {item.as_string()};
                                } else if (item.is_int()) {
                                        context->logger->trace("Array item for variable: '{}' is an integer with value: {}", variable_name, item.as_int());
                                        variable_list[variable_name] = {std::to_string(item.as_int())};
                                } else if (item.is_double()) {
                                        context->logger->trace("Array item for variable: '{}' is a double with value: {}", variable_name, item.as_double());
                                        variable_list[variable_name] = {std::to_string(item.as_double())};
                                } else if (item.is_bool()) {
                                        context->logger->trace("Array item for variable: '{}' is a boolean with value: {}", variable_name, item.as_bool());
                                        variable_list[variable_name] = {item.as_bool() ? "true" : "false"};
                                }
                                values.push_back(item.as_string());
                        }
                        variable_list[variable_name] = values;
                } else {
                        context->logger->error("Unsupported variable type for variable: {}", variable_name);
                        exit(1);
                }
        }
        return variable_list;
}

std::map<std::string,Profile> get_profiles(const Data& profiles) {
        std::map<std::string,Profile> profile_list;
        context->logger->debug("Processing profiles from Data object");
        for (const auto& [key, profile_value] : profiles.as_map()) {
                std::string profile_name = key;
                if (profile_name.empty()) {
                        context->logger->error("Profile name cannot be empty.");
                        exit(1);
                }
                if (!profile_value.is_map()) {
                        context->logger->error("Profile value must be a map for profile: {}", profile_name);
                        exit(1);
                }
                Profile profile(profile_name);
                context->logger->trace("Creating profile {}", profile_name);
                for (const auto& [var_key, var_value] : profile_value.as_map()) {
                        if (var_key == "variables") {
                                context->logger->trace("Adding variables to profile: {}", profile_name);
                                profile.add_variable(get_variables(var_value));
                        } else if (var_key == "ignore") {
                                context->logger->trace("Adding ignore items to profile: {}", profile_name);
                                profile.add_ignore(get_ignore(var_value));
                        } else if (var_key == "rules") {
                                context->logger->trace("Adding rules to profile: {}", profile_name);
                                profile.add_rules(get_rules(var_value));
                        } else {
                                context->logger->error("Unknown key '{}' in profile: {}", var_key, profile_name);
                                exit(1);
                        }
                }
                profile_list[profile_name] = profile;
        }
        return profile_list;
}

std::unordered_set<std::string> get_ignore(const Data& ignore) {
        std::unordered_set<std::string> ignore_list;
        context->logger->debug("Processing ignore items from Data object");
        if (!ignore.is_array()) {
                context->logger->error("Ignore must be an array.");
                exit(1);
        }
        for (const auto& item : ignore.as_array()) {
                if (!item.is_string()) {
                        context->logger->error("Ignore items must be strings.");
                        exit(1);
                }
                context->logger->trace("Adding ignore item: '{}'", item.as_string());
                ignore_list.insert(item.as_string());
        }
        return ignore_list;
}

std::map<std::string,std::string> get_rules(const Data& rules) {
        std::map<std::string,std::string> rule_set;
        context->logger->debug("Processing rules from Data object");
        if (!rules.is_map()) {
                context->logger->error("Rules must be a map.");
                exit(1);
        }
        for (const auto& [key, value] : rules.as_map()) {
                std::string rule_name = key;
                context->logger->trace("Processing rule: '{}'", rule_name);
                if (rule_name.empty()) {
                        context->logger->error("Rule name cannot be empty.");
                        exit(1);
                }
                if (!value.is_string() && !value.is_bool() && !value.is_int() && !value.is_double()) {
                        context->logger->error("Rule value must be a string, boolean, integer, or double for rule: {}", rule_name);
                        exit(1);
                }
                context->logger->trace("Rule: '{}' with value: '{}'", rule_name, value.as_string());
                rule_set[rule_name] = value.as_string();
        }
        return rule_set;
}


std::optional<std::filesystem::path> find_settings_file(const std::filesystem::path& dir) {
    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) return std::nullopt;

    const std::string target_stem = "settings";
    const std::vector<std::string> valid_extensions = {
        ".json", ".yaml", ".yml", ".xml", ".toml"
    };

    context->logger->info("Searching for settings file in directory: {}", dir.string());
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (!entry.is_regular_file()) continue;

        const std::filesystem::path& path = entry.path();
        if (path.stem() == target_stem) {
            context->logger->trace("File stem matches target: {}", path.stem().string());
            std::string ext = path.extension().string();
            if (std::find(valid_extensions.begin(), valid_extensions.end(), ext) != valid_extensions.end()) {
                context->logger->debug("Found settings file: {}", path.string());
                return path;
            }
        }
    }

    return std::nullopt;
}


void Prebyte::set_variable(const std::string& name, const std::string& value) {
        context->logger->trace("Setting variable '{}' to value '{}'", name, value);
        context->variables[name] = {value};
}

void Prebyte::set_variable(const std::string& name) {
        context->logger->trace("Setting variable '{}' to empty", name);
        context->variables[name] = {};
}

void Prebyte::set_variable(const std::string& name, std::vector<std::string> values) {
        context->logger->trace("Setting variable '{}' to values '{}'", name, values);
        context->variables[name] = values;
}

void Prebyte::set_profile(const std::string& profile_name) {
        Profile profile = context->profiles[profile_name];
        context->logger->trace("Setting profile: {}", profile_name);
        for (const auto& [key,value] : profile.get_variables()) {
                context->logger->trace("Setting variable '{}' to value '{}'", key, value);
                context->variables[key] = {value};
        }
        for (const auto& ignore_item : profile.get_ignore()) {
                context->ignore.insert(ignore_item);
        }
        for (const auto& [rule_name, rule_value] : profile.get_rules()) {
                context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_value)));
        }
}

void Prebyte::set_ignore(const std::string& ignore_item) {
        context->logger->trace("Adding ignore item: '{}'", ignore_item);
        context->ignore.insert(ignore_item);
}

void Prebyte::set_rule(const std::string& rule_name, const std::string& rule_value) {
        context->logger->trace("Setting rule '{}' to value '{}'", rule_name, rule_value);
        context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_value)));
}

std::string Prebyte::process(const std::string& input) {
        context->logger->debug("Processing input to return output");
        context->action_type = ActionType::API_IN_API_OUT;
        context->input = input;
        Preprocessor preprocessor(std::move(context));
        preprocessor.process();
        return context->output;
}

std::string Prebyte::process_file(const std::string& file_path) {
        context->logger->debug("Processing file to return output");
        context->action_type = ActionType::FILE_IN_API_OUT;
        context->logger->trace("Setting input file path: {}", file_path);
        context->inputs.push_back(file_path);
        Preprocessor preprocessor(std::move(context));
        preprocessor.process();
        return context->output;
}

void Prebyte::process(const std::string& input, const std::string& output_path) {
        context->logger->debug("Processing input into file");
        context->action_type = ActionType::API_IN_FILE_OUT;
        context->logger->trace("Setting output file path: {}", output_path);
        context->input = input;
        context->inputs.push_back(output_path);
        Preprocessor preprocessor(std::move(context));
        preprocessor.process();
}

void Prebyte::process_file(const std::string& file_path, const std::string& output_path) {
        context->logger->debug("Processing file into another file");
        context->action_type = ActionType::FILE_IN_FILE_OUT;
        context->logger->trace("Setting input file path: {}", file_path);
        context->logger->trace("Setting output file path: {}", output_path);
        context->inputs.push_back(file_path);
        context->inputs.push_back(output_path);
        Preprocessor preprocessor(std::move(context));
        preprocessor.process();
}

void Prebyte::set_logger() {
        if (context->logger) {
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

std::string Prebyte::expand_tilde(const std::string& path) {
    if (path.starts_with("~/")) {
        const char* home = std::getenv("HOME");
        if (home) {
            return std::string(home) + path.substr(1);
        }
    }
    return path;
}

}
