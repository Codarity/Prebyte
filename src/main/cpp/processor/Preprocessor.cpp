#include "processor/Preprocessor.h"

namespace prebyte {


Preprocessor::Preprocessor(std::unique_ptr<Context> context) : Processor(), process_variables(context.get()), process_flow(context.get()) {
    this->context = std::move(context);
}

void Preprocessor::process() {
        this->context->logger->info("Starting preprocessing...");
        this->input = get_input();
        if (input.empty()) {
                this->context->logger->warn("Input is empty. Exiting preprocessing.");
                return;
        }

        this->output = this->process_all(this->input);

        this->make_output();
        this->make_benchmark();
}

std::string Preprocessor::get_input() const {
    std::string input_data;
    this->context->logger->debug("Getting input for action type: " + std::to_string(static_cast<int>(context->action_type)));

    switch (context->action_type) {
        case ActionType::FILE_IN_FILE_OUT:
        case ActionType::FILE_IN_API_OUT:
        case ActionType::FILE_IN_STDOUT: {
            std::filesystem::path input_path = context->inputs[0];
            std::ifstream input_file(input_path);
            if (!input_file) {
                this->context->logger->error("Error opening input file: " + input_path.string());
                exit(1);
            }

            this->context->logger->debug("Reading input from file: " + input_path.string());

            input_data.assign(
                std::istreambuf_iterator<char>(input_file),
                std::istreambuf_iterator<char>()
            );

            if (input_data.empty()) {
                this->context->logger->warn("Input file is empty.");
                return "";
            }

            this->context->logger->debug("Input file read successfully.");
            break;
        }

        case ActionType::STDIN_FILE_OUT:
        case ActionType::STDIN_STDOUT: {
            std::ostringstream ss;
            ss << std::cin.rdbuf();
            input_data = ss.str();

            if (input_data.empty()) {
                this->context->logger->warn("No input provided on stdin.");
                return "";
            }
            break;
        }

        case ActionType::API_IN_FILE_OUT:
        case ActionType::API_IN_API_OUT: {
                if (context->input.empty()) {
                        this->context->logger->warn("No input provided for API action.");
                        return "";
                }
                this->context->logger->debug("Using provided input for API action.");
                return context->input;
        }
        default:
            this->context->logger->error("Unknown action type for input: " + std::to_string(static_cast<int>(context->action_type)));
            exit(1);
    }

    return input_data;
}

void Preprocessor::make_output() const {
        if (this->output.empty()) {
                this->context->logger->debug("Output is empty, nothing to write.");
                return;
        }
        this->context->logger->debug("Writing output for action type: " + std::to_string(static_cast<int>(context->action_type)));
        switch (context->action_type) {
                case ActionType::FILE_IN_FILE_OUT: {
                        std::filesystem::path output_path = context->inputs[1];
                        std::ofstream output_file(output_path);
                        if (!output_file) {
                                this->context->logger->error("Error opening output file: " + output_path.string());
                                exit(1);
                        }
                        this->context->logger->debug("Writing output to file: " + output_path.string());
                        output_file << output;
                        break;
                }
                case ActionType::API_IN_FILE_OUT:
                case ActionType::STDIN_FILE_OUT: {
                        std::filesystem::path output_path = context->inputs[0];
                        std::ofstream output_file(output_path);
                        if (!output_file) {
                                this->context->logger->error("Error opening output file: " + output_path.string());
                                exit(1);
                        }
                        this->context->logger->debug("Writing output to file: " + output_path.string());
                        output_file << output;
                        break;
                }
                case ActionType::FILE_IN_STDOUT:
                case ActionType::STDIN_STDOUT:
                        this->context->logger->debug("Writing output to stdout.");
                        std::cout << output;
                        break;
                case ActionType::API_IN_API_OUT:
                case ActionType::FILE_IN_API_OUT:
                        this->context->output = output;
                default:
                        this->context->logger->error("Unknown action type for output: " + std::to_string(static_cast<int>(context->action_type)));
                        exit(1);
        }
}

std::string Preprocessor::process_all(std::string input) {
        int position = -1;
        this->context->logger->debug("processing new input");
        std::string prefix = this->context->rules.variable_prefix.value();
        std::string suffix = this->context->rules.variable_suffix.value();
        std::string action_content = "";
        std::string output = "";
        while(!input.empty()) {
                position = input.find_first_of(this->context->rules.variable_prefix.value());
                if (position == std::string::npos) {
                        this->context->logger->trace("Variable prefix not found in input, adding remaining input to output.");
                        add_string(output,input);
                        input.clear();
                        continue;
                }
                add_string(output,input.substr(0, position));
                input.erase(0, position + this->context->rules.variable_prefix.value().length());

                if (!input.empty() && input[0] == '#') {
                        this->context->logger->debug("Found linewise action");
                        position = input.find_first_of('\n');
                        if (position == std::string::npos) {
                                position = input.length();
                        }
                        action_content = input.substr(1, position - 1);
                        input.erase(0, position + 1);
                } else {
                        this->context->logger->debug("Found inline action");
                        position = input.find_first_of(this->context->rules.variable_suffix.value());
                        if (position == std::string::npos) {
                                this->context->logger->error("Variable suffix not found in input.");
                                exit(1);
                        }
                        action_content = input.substr(0, position);
                        input.erase(0, position + this->context->rules.variable_suffix.value().length());
                }
                this->context->logger->debug("Found Action: " + action_content);
                add_string(output, do_action(action_content));
        }
        
        return output;
}

void Preprocessor::add_string(std::string& output, const std::string& str) {
        if (!this->ignore_next) {
                if (this->pipe) {
                        this->context->logger->trace("Adding string to pipe output");
                        this->output += str;
                } else {
                        this->context->logger->trace("Adding string to output");
                        output += str;
                }
        }
}



std::string Preprocessor::do_action(const std::string& action) {
        this->context->logger->debug("Processing action: " + action);
        std::string variable_name = get_variable_name(action);
        if (context->ignore.find(action) != context->ignore.end()) {
                this->context->logger->debug("Ignoring variable: " + action);
                return "";
        }
        if (process_variables.is_valid(action)) {
                this->context->logger->debug("Processing: " + action);
                return process_variables.get_value(action);
        }

        if (variable_name.starts_with("ARGS")) {
                this->context->logger->trace("Found ARGS variable");
                if (this->pipe) {
                        this->context->logger->trace("ARGS is redirected to pipe output");
                        return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                }
                if (this->macro_args.empty()) { 
                        this->context->logger->error("ARGS variable used without macro arguments.");
                        exit(1);
                }
                if (!action.ends_with("]")) {
                        this->context->logger->error("ARGS variable must be accessed with an index, e.g., ARGS[0].");
                        exit(1);
                }

                std::vector<std::string> args = this->macro_args.top();
                int index = get_variable_number(action);
                if (index < 0 || index >= args.size()) {
                        this->context->logger->error("Index out of bounds for ARGS variable: " + variable_name);
                        exit(1);
                }
                return args[index];
        }

        if (context->variables.find(variable_name) != context->variables.end()) {
                this->context->logger->debug("Substituting variable: " + variable_name);
                return get_variable(action);
        }

        if (process_flow.is_valid(action)) {
                if (this->pipe) {
                        this->context->logger->trace("Processing flow action in pipe mode: " + action);
                        if (action.starts_with("for ")) {
                                this->context->logger->trace("Processing 'for' action in pipe mode: " + action);
                                this->for_stack++;
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        } else if (action == "endfor" && this->for_stack > 1) {
                                this->context->logger->trace("Processing 'endfor' action in pipe mode: " + action);
                                this->for_stack--;
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        } else if (action.starts_with("include ")) {
                                this->context->logger->trace("Processing 'include' action in pipe mode: " + action);
                                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
                        }
                }
                return process_code_flow(action);
        }

        if (this->pipe) {
                this->context->logger->trace("Processing action in pipe mode: " + action);
                return this->context->rules.variable_prefix.value() + action + this->context->rules.variable_suffix.value();
        }

        if (this->ignore_next) {
                this->context->logger->debug("Ignoring action: " + action);
                return "";
        }

        if (context->rules.allow_env.value()) {
                this->context->logger->debug("Checking environment variables for action: " + action);
                if (action.starts_with("$")) {
                        const char* env_value = std::getenv(action.substr(1).c_str());
                        if (env_value) {
                                this->context->logger->debug("Found environment variable: " + action + " with value: " + std::string(env_value));
                                return std::string(env_value);
                        }
                }
                this->context->logger->debug("No environment variable found for action: {}. Checking fallback.", action);
                if (context->rules.allow_env_fallback.value()) {
                        const char* env_value = std::getenv(action.c_str());
                        if (env_value) {
                                this->context->logger->debug("Found environment variable: " + action + " with value: " + std::string(env_value));
                                return std::string(env_value);
                        }
                        this->context->logger->debug("No environment variable found for action: " + action);
                }
        }

        if (context->rules.set_default_variables.value()) {
                this->context->logger->debug("Using default variable value for action: " + action);
                return get_variable(context->rules.default_variable_value.value());
        }
        if (!context->rules.strict_variables.value()) {
                this->context->logger->debug("Variable '" + action + "' not found, returning as is.");
                return context->rules.variable_prefix.value() + action + context->rules.variable_suffix.value();
        } else {
                this->context->logger->error("Variable '" + action + "' not found and strict variables are enabled.");
                exit(1);
        }
}

std::string Preprocessor::process_code_flow(const std::string& action) {
        if (process_flow.is_skipable(action, ignore_next)) {
                this->context->logger->debug("Skipping action: " + action);
                return "";
        }

        std::string output = process_flow.get_value(action);
        FlowState this_state = process_flow.get_flow_state();

        this->context->logger->debug("Processing code flow action: {}", std::to_string(static_cast<int>(this_state)));

        if (this_state == FlowState::INCLUDE) {
                std::filesystem::path include_path = output;
                this->context->logger->debug("Including file: " + include_path.string());

                if (std::find(processed_includes.begin(), processed_includes.end(), include_path) != processed_includes.end()) {
                        this->context->logger->error("Circular include detected for " + include_path.string());
                        if (this->context->logger->should_log(spdlog::level::trace)) {
                                this->context->logger->trace("Processed includes:");
                                for (const auto& inc : processed_includes) {
                                        this->context->logger->trace(" - " + inc.string());
                                }
                        }
                        exit(1);
                }

                this->context->logger->trace("Adding include path to including stack: " + include_path.string());
                processed_includes.push_back(include_path);

                std::ifstream include_file(include_path);
                if (!include_file) {
                        this->context->logger->error("Error opening include file: " + include_path.string());
                        exit(1);
                }
                std::string include_content(
                    std::istreambuf_iterator<char>(include_file),
                    std::istreambuf_iterator<char>{}
                );

                this->context->include_counter++;
                this->context->logger->trace("Include depth: {}, total includes: {}", this->processed_includes.size(), this->context->include_counter);
                this->context->logger->debug("Processing included file: " + include_path.string());
                std::string result = process_all(include_content);
                this->context->logger->trace("Removing include path from including stack: " + include_path.string());
                processed_includes.pop_back();
                return result;
        } else if (this_state == FlowState::IF) {
                this->context->logger->debug("Processing 'if' condition: " + output);
                this->current_depth++;
                if (!ignore_next) {
                        this->context->logger->debug("Evaluating if condition: " + output);
                        if (output == "false") {
                                this->context->logger->debug("If condition evaluated to false, ignoring next text block");
                                this->ignore_next = true;
                                this->ignore_depth = this->current_depth;
                        } else {
                                this->context->logger->debug("If condition evaluated to true, ignoring all subsequent conditions until 'endif'");
                                this->ignore_all = this->current_depth;
                        }
                        return "";
                }
        } else if (this_state == FlowState::ELSE_IF) {
                this->context->logger->debug("Processing 'elif' condition: " + output);
                if (ignore_all) {
                        this->context->logger->debug("Ignoring elif condition, because there was a previous if/elif that was executed");
                        this->ignore_next = true;
                        return "";
                }
                if (this->current_depth == this->ignore_depth && this->ignore_next) {
                        this->context->logger->debug("Elif is at the same depth. Resetting ignore_next flag.");
                        this->ignore_next = false;
                }
                if (!ignore_next) {
                        this->context->logger->debug("Evaluating elif condition: " + output);
                        if (output == "false") {
                                this->context->logger->debug("Elif condition evaluated to false, ignoring next text block");
                                this->ignore_next = true;
                                this->ignore_depth = this->current_depth;
                        } else {
                                this->context->logger->debug("Elif condition evaluated to true, ignoring all subsequent conditions until 'endif'");
                                this->ignore_all = this->current_depth;
                        }
                        return "";
                }
        } else if (this_state == FlowState::ELSE) {
                this->context->logger->debug("Processing 'else' condition");
                if (ignore_all) {
                        this->context->logger->debug("Ignoring else condition, because there was a previous if/elif that was executed");
                        this->ignore_next = true;
                        return "";
                }
                if (this->current_depth == this->ignore_depth && this->ignore_next) {
                        this->context->logger->debug("Else is at the same depth. Resetting ignore_next flag.");
                        this->ignore_next = false;
                }
        } else if (this_state == FlowState::END_IF) {
                this->context->logger->debug("Processing 'endif' condition");
                this->current_depth--;
                this->ignore_all = 0;
                if (this->current_depth < 0) {
                        this->context->logger->error("Unmatched 'endif' in code flow.");
                        exit(1);
                }
                if (this->current_depth < this->ignore_depth) {
                        this->context->logger->debug("Resetting ignore_next flag, because current depth is less than ignore depth.");
                        this->ignore_next = false;
                }
        } else if (this_state == FlowState::DEFINE_PROFILE) {
                this->context->logger->debug("Defining a new Profile");
                this->pipe = true;
                this->output = "";
                this->profile_name = output.substr(0, output.find(' '));
                if (output.find(' ') == std::string::npos) {
                        this->context->logger->debug("No parser type specified, defaulting to YAML");
                        this->profile_parser_type = "yaml";
                } else {
                        this->profile_parser_type = output.substr(output.find(' ') + 1);
                        this->context->logger->debug("Profile parser type set to: " + this->profile_parser_type);
                }
                return "";
        } else if (this_state == FlowState::END_DEFINE) {
                this->context->logger->debug("Ending definition");
                this->pipe = false;
                if (!this->profile_name.empty()) {
                        StringParser profile_parser;
                        ParserType parser_type = profile_parser.getParserType(this->profile_parser_type);
                        Data profile_data = profile_parser.parse(this->output, parser_type);
                        Profile* profile = &context->profiles[this->profile_name];

                        if (profile_data.is_map()) {
                                this->context->logger->debug("Processing profile data for profile: " + this->profile_name);
                                for (const auto& [key, value] : profile_data.as_map()) {
                                        if (key == "variables") {
                                                this->context->logger->debug("Adding variables to profile: " + this->profile_name);
                                                profile->add_variable(this->get_variables(value));
                                        } else if (key == "ignore") {
                                                this->context->logger->debug("Adding ignore items to profile: " + this->profile_name);
                                                profile->add_ignore(this->get_ignore(value));
                                        } else if (key == "rules") {
                                                this->context->logger->debug("Adding rules to profile: " + this->profile_name);
                                                profile->add_rules(this->get_rules(value));
                                        } else {
                                                this->context->logger->warn("Unknown key in profile: " + key);
                                        }
                                }
                        } else {
                                this->context->logger->error("Invalid profile data format.");
                                exit(1);
                        }

                        this->context->logger->trace("Deleting profile name and output after definition");
                        this->profile_name.clear();
                        this->output.clear();
                } else if (!this->macro_name.empty()) {
                        this->context->logger->debug("Ending macro definition for " + this->macro_name);

                        if (context->macros.find(this->macro_name) != context->macros.end()) {
                                this->context->logger->warn("Macro '" + this->macro_name + "' already defined.");
                                return "";
                        }

                        this->context->logger->debug("Adding macro {}", this->macro_name);
                        context->macros[this->macro_name] = this->output;
                        this->context->logger->trace("Deleting macro name and output after definition");
                        this->macro_name.clear();
                        this->output.clear();
                } else {
                        this->context->logger->error("Found 'end_define' without a matching define.");
                        exit(1);
                }
                return "";
        } else if (this_state == FlowState::DEFINE_MACRO) {
                this->pipe = true;
                this->output = "";
                this->context->logger->debug("Defining new macro");
                this->macro_name = output;
                if (this->macro_name.empty()) {
                        this->context->logger->error("Macro name cannot be empty.");
                        exit(1);
                }
                this->context->logger->debug("Macro name set to: " + this->macro_name);
                return "";
        } else if (this_state == FlowState::EXECUTE_MACRO) {
                this->context->logger->trace("Try to parse macro execution command: " + output);
                std::string macro_name = output.substr(0, output.find(' '));
                output.erase(0, macro_name.length() + 1);
                if (macro_name.empty()) {
                        this->context->logger->error("Macro name cannot be empty.");
                        exit(1);
                }
                this->context->logger->debug("Executing macro: " + macro_name);
                this->macro_args.push(get_variable_values(output));

                this->context->logger->trace("Processing macro with name: " + macro_name);
                this->context->logger->trace("Macro arguments: ");
                if (this->context->logger->should_log(spdlog::level::trace)) {
                        for (const auto& arg : this->macro_args.top()) {
                                this->context->logger->trace(" - " + arg);
                        }
                }
                std::string result = process_all(this->context->macros[macro_name]);
                this->context->logger->trace("Popping macro arguments after execution");
                this->macro_args.pop();

                return result;
        } else if (this_state == FlowState::FOR) {
                this->context->logger->debug("Processing 'for' loop: " + output);
                this->for_stack++;
                if (this->for_stack == 1) {
                        this->context->logger->debug("Starting new for");
                        this->pipe = true;
                        this->output = "";
                        this->for_variable = output;
                        output = "";
                } else {
                        this->context->logger->debug("Nested for loop detected, incrementing for stack: " + std::to_string(this->for_stack));
                }
        } else if (this_state == FlowState::END_FOR) {
                this->context->logger->debug("Processing 'endfor' action");
                this->for_stack--;
                if (this->for_stack == 0) {
                        this->context->logger->debug("Ending for loop, ending pipe mode");
                        this->pipe = false;
                        std::string for_variable = this->for_variable.substr(0, this->for_variable.find(' '));
                        std::string for_array = this->for_variable.substr(this->for_variable.find_last_of(' ') + 1);

                        this->context->logger->trace("Trying to process for loop variables");
                        if (for_variable.empty() || for_array.empty()) {
                                this->context->logger->error("For loop variable or array cannot be empty.");
                                exit(1);
                        }

                        this->context->logger->trace("Trying to find array for for loop: " + for_array);

                        if (context->variables.find(for_array) == context->variables.end()) {
                                this->context->logger->error("For loop array '" + for_array + "' not found.");
                                exit(1);
                        }

                        this->context->logger->debug("For loop variable: " + for_variable);
                        this->context->logger->debug("For loop array: " + for_array);

                        std::vector<std::string> values = context->variables[for_array];
                        if (values.empty()) {
                                this->context->logger->debug("For loop array is empty, nothing to iterate over.");
                                return "";
                        }

                        std::string to_loop = this->output;
                        std::string result;

                        this->context->logger->debug("Processing for loop with " + std::to_string(values.size()) + " items.");
                        for (const std::string value : values) {
                                this->context->logger->trace("Processing for loop value: " + value);
                                context->variables[for_variable] = {value};
                                this->context->logger->trace("Substituting for loop variable: " + for_variable + " with value: " + value);
                                result += process_all(to_loop);
                        }

                        this->context->logger->trace("For loop completed, resetting output and for_variable.");
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
                this->context->logger->trace("Processing variable: " + variable);
                if (variable.starts_with("\"")) {
                        this->context->logger->trace("Variable is a quoted string");
                        size_t end_quote = variable.find('"', 1);
                        if (end_quote == std::string::npos) {
                                this->context->logger->error("Unmatched quote in variable: " + variable);
                                exit(1);
                        }
                        result.push_back(variable.substr(1, end_quote - 1));
                        variable.erase(0, end_quote + 1);
                        this->context->logger->trace("Extracted quoted string: " + result.back());
                } else if (variable.ends_with("#")) {
                        this->context->logger->trace("Variable ends with '#', getting size of array variable");
                        result.push_back(std::to_string(context->variables[variable.substr(0, variable.length() - 1)].size()));
                        variable.erase(0, variable.length());
                        this->context->logger->trace("Extracted size of array variable: " + result.back());
                } else {
                        this->context->logger->trace("Variable is a normal variable or array access");
                        size_t next_space = variable.find_first_of(" ");
                        std::string variable_name;
                        std::string variable_value;
                        if (next_space == std::string::npos) {
                                this->context->logger->trace("No space found in variable, using entire variable as name");
                                variable_name = variable;
                                variable.erase(0, variable.length());
                        } else {
                                this->context->logger->trace("Space found in variable, splitting at first space");
                                variable_name = variable.substr(0, next_space);
                                variable.erase(0, next_space);
                        }

                        int index = 0;
                        this->context->logger->trace("Checking if variable is an array access");
                        if (variable_name.ends_with("]")) {
                                this->context->logger->trace("Variable is an array access");
                                index = get_variable_number(variable_name);
                                variable_name = get_variable_name(variable_name);
                        } else {
                                this->context->logger->trace("Variable is not an array access");
                        }

                        this->context->logger->trace("Checking if variable exists");
                        if (context->variables.find(variable_name) != context->variables.end()) {
                                if (index < 0 || index >= context->variables[variable_name].size()) {
                                        this->context->logger->error("Index out of bounds for variable: " + variable_name);
                                        exit(1);
                                }
                                variable_value = context->variables[variable_name][index];
                                this->context->logger->trace("Found variable: " + variable_name + " with value: " + variable_value);
                        } else {
                                variable_value = variable_name;
                        }

                        this->context->logger->trace("Adding variable value to result: " + variable_value);
                        result.push_back(variable_value);
                }
                this->context->logger->trace("Trimming variable string: " + variable);
                variable.erase(0, variable.find_first_not_of(" \t\n\r\f\v"));
        }
        return result;
}


std::map<std::string,std::vector<std::string>> Preprocessor::get_variables(const Data& variables) {
        std::map<std::string,std::vector<std::string>> variable_list;
        this->context->logger->trace("Processing variables from Data object");
        for (const auto& [key, value] : variables.as_map()) {
                std::string variable_name = key;
                if (variable_name.empty()) {
                        this->context->logger->error("Variable name cannot be empty.");
                        exit(1);
                }
                if (value.is_null()) {
                        this->context->logger->error("Variable value cannot be null for variable: " + variable_name);
                        exit(1);
                }

                this->context->logger->trace("Processing variable: " + variable_name);
                if (value.is_string()) {
                        this->context->logger->trace("Variable is a string: " + value.as_string());
                        variable_list[variable_name] = {value.as_string()};
                } else if (value.is_int()) {
                        this->context->logger->trace("Variable is an integer: " + std::to_string(value.as_int()));
                        variable_list[variable_name] = {std::to_string(value.as_int())};
                } else if (value.is_double()) {
                        this->context->logger->trace("Variable is a double: " + std::to_string(value.as_double()));
                        variable_list[variable_name] = {std::to_string(value.as_double())};
                } else if (value.is_bool()) {
                        this->context->logger->trace("Variable is a boolean: " + std::string(value.as_bool() ? "true" : "false"));
                        variable_list[variable_name] = {value.as_bool() ? "true" : "false"};
                } else if (value.is_array()) {
                        this->context->logger->trace("Variable is an array");
                        std::vector<std::string> values;
                        for (const auto& item : value.as_array()) {
                                if (item.is_null() || item.is_array() || item.is_map()) {
                                        this->context->logger->error("Variable value cannot be null, array, or map for variable: " + variable_name);
                                        exit(1);
                                }
                                if (item.is_string()) {
                                        this->context->logger->trace("Array item is a string: " + item.as_string());
                                        variable_list[variable_name] = {item.as_string()};
                                } else if (item.is_int()) {
                                        this->context->logger->trace("Array item is an integer: " + std::to_string(item.as_int()));
                                        variable_list[variable_name] = {std::to_string(item.as_int())};
                                } else if (item.is_double()) {
                                        this->context->logger->trace("Array item is a double: " + std::to_string(item.as_double()));
                                        variable_list[variable_name] = {std::to_string(item.as_double())};
                                } else if (item.is_bool()) {
                                        this->context->logger->trace("Array item is a boolean: " + std::string(item.as_bool() ? "true" : "false"));
                                        variable_list[variable_name] = {item.as_bool() ? "true" : "false"};
                                }
                                values.push_back(item.as_string());
                        }
                        this->context->logger->trace("Adding array values to variable: " + variable_name);
                        variable_list[variable_name] = values;
                } else {
                        this->context->logger->error("Unsupported variable type for variable: " + variable_name);
                        exit(1);
                }
        }
        return variable_list;
}


std::unordered_set<std::string> Preprocessor::get_ignore(const Data& ignore) {
        std::unordered_set<std::string> ignore_list;
        this->context->logger->trace("Processing ignore list from Data object");
        if (!ignore.is_array()) {
                this->context->logger->error("Ignore must be an array.");
                exit(1);
        }
        for (const auto& item : ignore.as_array()) {
                if (!item.is_string()) {
                        this->context->logger->error("Ignore items must be strings.");
                        exit(1);
                }
                this->context->logger->trace("Adding item to ignore list: " + item.as_string());
                ignore_list.insert(item.as_string());
        }
        return ignore_list;
}

std::map<std::string,std::string> Preprocessor::get_rules(const Data& rules) {
        std::map<std::string,std::string> rule_set;
        if (!rules.is_map()) {
                this->context->logger->error("Rules must be a map.");
                exit(1);
        }
        for (const auto& [key, value] : rules.as_map()) {
                std::string rule_name = key;
                if (rule_name.empty()) {
                        this->context->logger->error("Rule name cannot be empty.");
                        exit(1);
                }
                if (!value.is_string() && !value.is_bool() && !value.is_int() && !value.is_double()) {
                        this->context->logger->error("Rule value must be a string, boolean, integer, or double for rule: " + rule_name);
                        exit(1);
                }
                this->context->logger->trace("Processing rule {} for name: {}", rule_name, value.as_string());
                rule_set[rule_name] = value.as_string();
        }
        return rule_set;
}

void Preprocessor::make_benchmark() const {
        this->context->logger->debug("Making benchmark results");
        if (context->rules.benchmark.value() == Benchmark::NONE) {
                this->context->logger->debug("Benchmarking is disabled, skipping results.");
                return;
        }
        std::cout << "______________________________________________________________" << std::endl;
        std::cout << "| Benchmarking results:                                      |" << std::endl;
        std::cout << "______________________________________________________________" << std::endl;
        if (context->rules.benchmark.value() == Benchmark::TIME || context->rules.benchmark.value() == Benchmark::ALL) {
                std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - context->start_time);
                std::cout << "Time: " << this->get_time_conversion(duration) << std::endl;
        }
        std::cout << "Includes processed: " << context->include_counter << std::endl;
        std::cout << "Current Variables set: " << context->variables.size() << " variables." << std::endl;
}

std::string Preprocessor::get_time_conversion(const std::chrono::nanoseconds& duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration - seconds);
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - seconds - milliseconds);

    if (seconds.count() > 0) {
            return std::to_string(seconds.count()) + "." + std::to_string(milliseconds.count()) + "s";
    } else if (milliseconds.count() > 0) {
            return std::to_string(milliseconds.count()) + "." + std::to_string(nanoseconds.count()) + "ms";
    } else {
            return std::to_string(nanoseconds.count()) + "ns";
    }
}


}
