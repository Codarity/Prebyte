#include "processor/Processor.h"

namespace prebyte {


std::string Processor::get_variable_value(const std::string& action, bool pattern) const {
    static const std::regex var_pattern(
        R"(^([a-zA-Z][a-zA-Z0-9_]*)(?:\[(\d+)\])?$)"
    );

    std::smatch match;
    if (std::regex_match(action, match, var_pattern)) {
        if (pattern && match[2].matched) return match[2];
        if (!pattern) return match[1];
    }

    return "";
}

std::string Processor::get_variable_name(const std::string& action) const {
        return get_variable_value(action, false);
}

int Processor::get_variable_number(const std::string& action) const {
    std::string num = get_variable_value(action, true);
    if (num.empty()) return 0;
    return std::stoi(num);
}

std::string Processor::get_variable(const std::string& variable_name) const {
        std::string variable = get_variable_name(variable_name);
        int index = 0;
        if (variable_name.ends_with("]")) {
                index = get_variable_number(variable_name);
        }

        if (index + 1 > context->variables[variable].size()) variable = "";
        else variable = context->variables[variable][index];

        if (variable.starts_with("@")) {
                if (variable.starts_with("@@")) {
                        variable.erase(0, 1);
                } else {
                        variable.erase(0, 1);
                        if (std::filesystem::exists(variable)) {
                                std::ifstream file(variable);
                                if (!file) {
                                        this->context->logger->error("Error opening file {} for variable {}", variable, variable_name);
                                        end(context.get());
                                }
                                variable.assign(
                                    std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>()
                                );
                        } else {
                                this->context->logger->error("File ({}) does not exist for variable: {}", variable, variable_name);
                                end(context.get());
                        }
                }
        }
        

        if (context->rules.trim_start.value()) {
                variable.erase(0, variable.find_first_not_of(' '));
        }
        if (context->rules.trim_end.value()) {
                variable.erase(variable.find_last_not_of(' ') + 1);
        }

        if (context->rules.max_variable_length.value() < 0 || variable.length() <= context->rules.max_variable_length.value()) return variable;
        return variable_name.substr(0, context->rules.max_variable_length.value());
}

}
