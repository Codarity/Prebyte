#include "processor/Preprocessor.h"

namespace prebyte {

void Preprocessor::process() {
        this->input = get_input();
        if (input.empty()) {
                return;
        }

        this->process_all();

        this->make_output();
}

std::string Preprocessor::get_input() const {
    std::string input_data;

    switch (context.action_type) {
        case ActionType::FILE_IN_FILE_OUT:
        case ActionType::FILE_IN_STDOUT: {
            std::filesystem::path input_path = context.inputs[0];
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
        switch (context.action_type) {
                case ActionType::FILE_IN_FILE_OUT: {
                        std::filesystem::path output_path = context.inputs[1];
                        std::ofstream output_file(output_path);
                        if (!output_file) {
                                std::cerr << "Error opening output file: " << output_path << std::endl;
                                return;
                        }
                        output_file << output;
                        break;
                }
                case ActionType::STDIN_FILE_OUT: {
                        std::filesystem::path output_path = context.inputs[0];
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

void Preprocessor::process_all() {
        int position = -1;
        std::string prefix = this->context.rules.variable_prefix;
        std::string suffix = this->context.rules.variable_suffix;
        std::string action_content = "";
        while(!input.empty()) {
                position = input.find_first_of(this->context.rules.variable_prefix);
                if (position == std::string::npos) {
                        output += input;
                        input.clear();
                        continue;
                }
                output += input.substr(0, position);
                input.erase(0, position + this->context.rules.variable_prefix.length());

                position = input.find_first_of(this->context.rules.variable_suffix);
                if (position == std::string::npos) {
                        std::cerr << "Error: Unmatched variable prefix in input." << std::endl;
                        output += this->context.rules.variable_prefix + input;
                        input.clear();
                        continue;
                }
                action_content = input.substr(0, position);
                input.erase(0, position + this->context.rules.variable_suffix.length());
                do_action(action_content);
        }
}

void Preprocessor::do_action(const std::string& action) {
        if (context.ignore.find(action) != context.ignore.end()) {
                return;
        }
        if (context.variables.find(action) != context.variables.end()) {
                output += get_variable(context.variables[action]);
                return;
        }

        if (context.rules.allow_env) {
                const char* env_value = std::getenv(action.c_str());
                if (env_value) {
                        output += std::string(env_value);
                        return;
                }
        }

        if (context.rules.set_default_variables) {
                output += get_variable(context.rules.default_variable_value);
                return;
        }
        if (!context.rules.strict_variables) {
                output += context.rules.variable_prefix + action + context.rules.variable_suffix;
        } else {
                std::cerr << "Error: Variable '" << action << "' not found." << std::endl;
                exit(1);
        }
}

}
