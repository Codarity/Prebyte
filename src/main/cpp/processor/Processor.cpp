#include "processor/Processor.h"

namespace prebyte {

std::string Processor::get_variable(const std::string& variable_name) const {
        std::string variable = variable_name;

        if (variable.starts_with("@")) {
                if (variable.starts_with("@@")) {
                        variable.erase(0, 1);
                } else {
                        variable.erase(0, 1);
                        if (std::filesystem::exists(variable)) {
                                std::ifstream file(variable);
                                if (!file) {
                                        throw std::runtime_error("Error opening file: " + variable);
                                }
                                variable.assign(
                                    std::istreambuf_iterator<char>(file),
                                    std::istreambuf_iterator<char>()
                                );
                        } else {
                                throw std::runtime_error("File does not exist: " + variable);
                        }
                }
        }

        if (context.rules.trim_start) {
                variable.erase(0, variable.find_first_not_of(' '));
        }
        if (context.rules.trim_end) {
                variable.erase(variable.find_last_not_of(' ') + 1);
        }

        if (context.rules.max_variable_length < 0 || variable.length() <= context.rules.max_variable_length) return variable;
        return variable_name.substr(0, context.rules.max_variable_length);
}

}
