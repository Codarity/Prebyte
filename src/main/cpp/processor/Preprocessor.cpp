#include "processor/Preprocessor.h"

namespace prebyte {


Preprocessor::Preprocessor(Context* context) : Processor(), process_variables(context), process_flow(context) {
    this->context = context;
}

void Preprocessor::process() {
        this->input = get_input();
        if (input.empty()) {
                return;
        }

        this->output = this->process_all(this->input);

        this->make_output();
}

std::string Preprocessor::get_input() const {
    std::string input_data;

    switch (context->action_type) {
        case ActionType::FILE_IN_FILE_OUT:
        case ActionType::FILE_IN_STDOUT: {
            std::filesystem::path input_path = context->inputs[0];
            std::ifstream input_file(input_path);
            if (!input_file) {
                std::cerr << "Error opening input file: " << input_path << std::endl;
                return "";
            }

            input_data.assign(
                std::istreambuf_iterator<char>(input_file),
                std::istreambuf_iterator<char>()
            );

            if (input_data.empty()) {
                std::cerr << "Error: Input file is empty." << std::endl;
                return "";
            }
            break;
        }

        case ActionType::STDIN_FILE_OUT:
        case ActionType::STDIN_STDOUT: {
            std::ostringstream ss;
            ss << std::cin.rdbuf();
            input_data = ss.str();

            if (input_data.empty()) {
                std::cerr << "Error: No input provided on stdin." << std::endl;
                return "";
            }
            break;
        }

        default:
            std::cerr << "Error: Unsupported action type." << std::endl;
            return "";
    }

    return input_data;
}

void Preprocessor::make_output() const {
        switch (context->action_type) {
                case ActionType::FILE_IN_FILE_OUT: {
                        std::filesystem::path output_path = context->inputs[1];
                        std::ofstream output_file(output_path);
                        if (!output_file) {
                                std::cerr << "Error opening output file: " << output_path << std::endl;
                                return;
                        }
                        output_file << output;
                        break;
                }
                case ActionType::STDIN_FILE_OUT: {
                        std::filesystem::path output_path = context->inputs[0];
                        std::ofstream output_file(output_path);
                        if (!output_file) {
                                std::cerr << "Error opening output file: " << output_path << std::endl;
                                return;
                        }
                        output_file << output;
                        break;
                }
                case ActionType::FILE_IN_STDOUT:
                case ActionType::STDIN_STDOUT:
                        std::cout << output;
                        break;
                default:
                        std::cerr << "Unknown action type." << std::endl;
                        break;
        }
}

std::string Preprocessor::process_all(std::string input) {
        int position = -1;
        std::string prefix = this->context->rules.variable_prefix.value();
        std::string suffix = this->context->rules.variable_suffix.value();
        std::string action_content = "";
        std::string output = "";
        while(!input.empty()) {
                position = input.find_first_of(this->context->rules.variable_prefix.value());
                if (position == std::string::npos) {
                        add_string(output,input);
                        input.clear();
                        continue;
                }
                add_string(output,input.substr(0, position));
                input.erase(0, position + this->context->rules.variable_prefix.value().length());

                if (!input.empty() && input[0] == '#') {
                        position = input.find_first_of('\n');
                        if (position == std::string::npos) {
                                position = input.length();
                        }
                        action_content = input.substr(1, position - 1);
                        input.erase(0, position + 1);
                } else {
                        position = input.find_first_of(this->context->rules.variable_suffix.value());
                        if (position == std::string::npos) {
                                std::cerr << "Error: Unmatched variable prefix in input." << std::endl;
                                add_string(output,this->context->rules.variable_prefix.value() + input);
                                input.clear();
                                continue;
                        }
                        action_content = input.substr(0, position);
                        input.erase(0, position + this->context->rules.variable_suffix.value().length());
                }
                add_string(output, do_action(action_content));
        }
        
        return output;
}

void Preprocessor::add_string(std::string& output, const std::string& str) {
        if (!this->ignore_next) {
                if (this->pipe) {
                        this->output += str;
                } else {
                        output += str;
                }
        }
}



std::string Preprocessor::do_action(const std::string& action) {
        std::string variable_name = get_variable_name(action);
        if (context->ignore.find(action) != context->ignore.end()) {
                return "";
        }
        if (process_variables.is_valid(action)) {
                return process_variables.get_value(action);
        }

        if (variable_name.starts_with("ARGS")) {
                if (this->pipe) {
                        return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                }
                if (this->macro_args.empty() || !action.ends_with("]")) {
                        std::cerr << "Error: No macro arguments available." << std::endl;
                        return "";
                }

                std::vector<std::string> args = this->macro_args.top();
                int index = get_variable_number(action);
                if (index < 0 || index >= args.size()) {
                        std::cerr << "Error: Index out of bounds for ARGS variable: " << variable_name << std::endl;
                        return "";
                }
                return args[index];
        }

        if (context->variables.find(variable_name) != context->variables.end()) {
                return get_variable(action);
        }

        if (process_flow.is_valid(action)) {
                if (this->pipe) {
                        if (action.starts_with("for ")) {
                                this->for_stack++;
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        } else if (action == "endfor" && this->for_stack > 1) {
                                this->for_stack--;
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        } else if (action.starts_with("include ")) {
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        }
                }
                return process_code_flow(action);
        }

        if (this->pipe) return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();

        if (this->ignore_next) return "";

        if (context->rules.allow_env.value()) {
                if (action.starts_with("$")) {
                        const char* env_value = std::getenv(action.substr(1).c_str());
                        if (env_value) {
                                return std::string(env_value);
                        }
                }
                if (context->rules.allow_env_fallback.value()) {
                        const char* env_value = std::getenv(action.c_str());
                        if (env_value) {
                                return std::string(env_value);
                        }
                }
        }

        if (context->rules.set_default_variables.value()) {
                return get_variable(context->rules.default_variable_value.value());
        }
        if (!context->rules.strict_variables.value()) {
                return context->rules.variable_prefix.value() + action + context->rules.variable_suffix.value();
        } else {
                std::cerr << "Error: Variable '" << action << "' not found." << std::endl;
                exit(1);
        }
        return "";
}

std::string Preprocessor::process_code_flow(const std::string& action) {
        if (process_flow.is_skipable(action, ignore_next)) {
                return "";
        }
        std::string output = process_flow.get_value(action);
        FlowState this_state = process_flow.get_flow_state();
        if (this_state == FlowState::INCLUDE) {
                std::filesystem::path include_path = output;
                if (std::find(processed_includes.begin(), processed_includes.end(), include_path) != processed_includes.end()) {
                        std::cerr << "Error: Circular include detected for " << include_path << std::endl;
                        return "";
                }
                processed_includes.push_back(include_path);

                std::ifstream include_file(include_path);
                if (!include_file) {
                        std::cerr << "Error opening include file: " << include_path << std::endl;
                        return "";
                }
                std::string include_content(
                    std::istreambuf_iterator<char>(include_file),
                    std::istreambuf_iterator<char>{}
                );
                std::string result = process_all(include_content);
                processed_includes.pop_back();
                return result;
        } else if (this_state == FlowState::IF) {
                this->current_depth++;
                if (!ignore_next) {
                        if (output == "false") {
                                this->ignore_next = true;
                                this->ignore_depth = this->current_depth;
                        } else {
                                this->ignore_all = this->current_depth;
                        }
                        return "";
                }
        } else if (this_state == FlowState::ELSE_IF) {
                if (ignore_all) {
                        this->ignore_next = true;
                        return "";
                }
                if (this->current_depth == this->ignore_depth && this->ignore_next) {
                        this->ignore_next = false;
                }
                if (!ignore_next) {
                        if (output == "false") {
                                this->ignore_next = true;
                                this->ignore_depth = this->current_depth;
                        } else {
                                this->ignore_all = this->current_depth;
                        }
                        return "";
                }
        } else if (this_state == FlowState::ELSE) {
                if (ignore_all) {
                        this->ignore_next = true;
                        return "";
                }
                if (this->current_depth == this->ignore_depth && this->ignore_next) {
                        this->ignore_next = false;
                }
        } else if (this_state == FlowState::END_IF) {
                this->current_depth--;
                this->ignore_all = 0;
                if (this->current_depth < 0) {
                        std::cerr << "Error: Unmatched 'endif' in code flow." << std::endl;
                        return "";
                }
                if (this->current_depth < this->ignore_depth) {
                        this->ignore_next = false;
                }
        } else if (this_state == FlowState::DEFINE_PROFILE) {
                this->pipe = true;
                this->output = "";
                this->profile_name = output.substr(0, output.find(' '));
                if (output.find(' ') == std::string::npos) {
                        this->profile_parser_type = "yaml";
                } else {
                        this->profile_parser_type = output.substr(output.find(' ') + 1);
                }
                return "";
        } else if (this_state == FlowState::END_DEFINE) {
                this->pipe = false;
                if (!this->profile_name.empty()) {
                        StringParser profile_parser;
                        ParserType parser_type = profile_parser.getParserType(this->profile_parser_type);
                        Data profile_data = profile_parser.parse(this->output, parser_type);
                        Profile* profile = &context->profiles[this->profile_name];

                        if (profile_data.is_map()) {
                                for (const auto& [key, value] : profile_data.as_map()) {
                                        if (key == "variables") {
                                                profile->add_variable(this->get_variables(value));
                                        } else if (key == "ignore") {
                                                profile->add_ignore(this->get_ignore(value));
                                        } else if (key == "rules") {
                                                profile->add_rules(this->get_rules(value));
                                        } else {
                                                std::cerr << "Warning: Unknown key in profile: " << key << std::endl;
                                        }
                                }
                        } else {
                                std::cerr << "Error: Invalid profile data format." << std::endl;
                        }

                        this->profile_name.clear();
                        this->output.clear();
                } else if (!this->macro_name.empty()) {
                        if (context->macros.find(this->macro_name) != context->macros.end()) {
                                std::cerr << "Error: Macro '" << this->macro_name << "' already defined." << std::endl;
                                return "";
                        }
                        context->macros[this->macro_name] = this->output;
                        this->macro_name.clear();
                        this->output.clear();
                } else {
                        std::cerr << "Error: No macro or profile defined." << std::endl;
                }
                return "";
        } else if (this_state == FlowState::DEFINE_MACRO) {
                this->pipe = true;
                this->output = "";
                this->macro_name = output;
                return "";
        } else if (this_state == FlowState::EXECUTE_MACRO) {
                std::string macro_name = output.substr(0, output.find(' '));
                output.erase(0, macro_name.length() + 1);
                if (macro_name.empty()) {
                        std::cerr << "Error: Macro name cannot be empty." << std::endl;
                        return "";
                }
                this->macro_args.push(get_variable_values(output));

                std::string result = process_all(this->context->macros[macro_name]);
                this->macro_args.pop();

                return result;
        } else if (this_state == FlowState::FOR) {
                this->for_stack++;
                if (this->for_stack == 1) {
                        this->pipe = true;
                        this->output = "";
                        this->for_variable = output;
                        output = "";
                }
        } else if (this_state == FlowState::END_FOR) {
                this->for_stack--;
                if (this->for_stack == 0) {
                        this->pipe = false;
                        std::string for_variable = this->for_variable.substr(0, this->for_variable.find(' '));
                        std::string for_array = this->for_variable.substr(this->for_variable.find_last_of(' ') + 1);
                        if (for_variable.empty() || for_array.empty()) {
                                std::cerr << "Error: For loop variable or array cannot be empty." << std::endl;
                                return "";
                        }
                        if (context->variables.find(for_array) == context->variables.end()) {
                                std::cerr << "Error: For loop array '" << for_array << "' not found." << std::endl;
                                return "";
                        }
                        std::vector<std::string> values = context->variables[for_array];
                        if (values.empty()) {
                                return "";
                        }
                        std::string to_loop = this->output;
                        std::string result;
                        for (const std::string value : values) {
                                context->variables[for_variable] = {value};
                                result += process_all(to_loop);
                        }
                        this->output.clear();
                        this->for_variable.clear();
                        output = result;
                }
        }
        return output;
}

std::vector<std::string> Preprocessor::get_variable_values(std::string variable) {
        std::vector<std::string> result;
        while (!variable.empty()) {
                if (variable.starts_with("\"")) {
                        size_t end_quote = variable.find('"', 1);
                        if (end_quote == std::string::npos) {
                                throw std::runtime_error("Unmatched quote in variable: " + variable);
                        }
                        result.push_back(variable.substr(1, end_quote - 1));
                        variable.erase(0, end_quote + 1);
                } else if (variable.ends_with("#")) {
                        result.push_back(std::to_string(context->variables[variable.substr(0, variable.length() - 1)].size()));
                        variable.erase(0, variable.length() - 1);
                } else {
                        size_t next_space = variable.find_first_of(" ");
                        std::string variable_name;
                        std::string variable_value;
                        if (next_space == std::string::npos) {
                                variable_name = variable;
                                variable.erase(0, variable.length());
                        } else {
                                variable_name = variable.substr(0, next_space);
                                variable.erase(0, next_space);
                        }

                        int index = 0;
                        if (variable_name.ends_with("]")) {
                                index = get_variable_number(variable_name);
                                variable_name = get_variable_name(variable_name);
                        }

                        if (context->variables.find(variable_name) != context->variables.end()) {
                                if (index < 0 || index >= context->variables[variable_name].size()) {
                                        throw std::runtime_error("Index out of bounds for variable: " + variable_name);
                                }
                                variable_value = context->variables[variable_name][index];
                        } else {
                                variable_value = variable_name;
                        }
                        result.push_back(variable_value);
                }
                variable.erase(0, variable.find_first_not_of(" \t\n\r\f\v"));
        }
        return result;
}


std::map<std::string,std::vector<std::string>> Preprocessor::get_variables(const Data& variables) {
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


std::unordered_set<std::string> Preprocessor::get_ignore(const Data& ignore) {
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

std::map<std::string,std::string> Preprocessor::get_rules(const Data& rules) {
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


}
