#include "processor/Metaprocessor.h"

namespace prebyte {

Metaprocessor::Metaprocessor(Context* context) : Processor() {
        this->context = context;
}

void Metaprocessor::process() {
        switch (context->action_type) {
        case ActionType::EXPLAIN:
                explain();
                break;
        case ActionType::HELP:
                help();
                break;
        case ActionType::HARD_HELP:
                hard_help();
                break;
        case ActionType::VERSION:
                version();
                break;
        case ActionType::LIST_RULES:
                list_rules();
                break;
        case ActionType::LIST_VARIABLES:
                list_variables();
                break;
        default:
                std::cerr << "Unknown action type: " << static_cast<int>(context->action_type) << std::endl;
                help();
                break;
        }
}

void Metaprocessor::explain() {
}

void Metaprocessor::help() {
        std::cout << "Usage: prebyte <command> [options]\n"
         << "Prebyte is a tool for processing files with various commands.\n"
         << "You can use it to read files, write output, and manage rules and variables.\n\n"
         << "If no input file is specified, prebyte will read from standard input and write to standard output.\n"
         << "If an input file is specified, prebyte will read from that file and write to standard output by default.\n\n"

         << "Available commands:\n"
         << "  <file>                  Process the specified file\n"
         << "  -o, --output <file>     Write output to the specified file\n"
         << "  <file> -o <file>        Read from the specified file and write output to another file\n"
         << "  -h, --help              Show this help message\n"
         << "  -v, --version           Show version information\n"
         << "  -e, --explain           Explain the usage of the tool\n"
         << "  -lsr, --list-rules      List used rules\n"
         << "  -lsv, --list-variables  List used variables\n";
}

void Metaprocessor::hard_help() {
        std::cerr << "Error: Invalid command or options provided.\n"
         << "Use -h or --help to see the usage instructions.\n";
        std::exit(1);
}

void Metaprocessor::version() {
        std::cout << "Prebyte Version: 1.0.0" << std::endl;
}

void Metaprocessor::list_rules() {
        std::string rules_list;
        rules_list += "strict_variables: " + std::string(context->rules.strict_variables.value() ? "true" : "false") + "\n";
        rules_list += "set_default_variables: " + std::string(context->rules.set_default_variables.value() ? "true" : "false") + "\n";
        rules_list += "trim_start: " + std::string(context->rules.trim_start.value() ? "true" : "false") + "\n";
        rules_list += "trim_end: " + std::string(context->rules.trim_end.value() ? "true" : "false") + "\n";
        rules_list += "allow_env: " + std::string(context->rules.allow_env.value() ? "true" : "false") + "\n";
        rules_list += "allow_env_fallback: " + std::string(context->rules.allow_env_fallback.value() ? "true" : "false") + "\n";
        rules_list += "debug_level: ";
        rules_list +=
            (context->rules.debug_level.value() == DebugLevel::ERROR   ? "[ERROR] "
             : context->rules.debug_level.value() == DebugLevel::WARNING ? "[WARNING] "
             : context->rules.debug_level.value() == DebugLevel::INFO    ? "[INFO] "
             : "[DEBUG] ");
        rules_list += "\n";

        if (context->rules.max_variable_length.value() != -1) {
            rules_list += "max_variable_length: " + std::to_string(context->rules.max_variable_length.value()) + "\n";
        } else {
                rules_list += "max_variable_length: No Limit\n";
                }
        rules_list += "default_variable_value: " + context->rules.default_variable_value.value() + "\n";
        rules_list += "variable_prefix: " + context->rules.variable_prefix.value() + "\n";
        rules_list += "variable_suffix: " + context->rules.variable_suffix.value() + "\n";
        rules_list += "include_path: " + context->rules.include_path.value() + "\n";
        rules_list += "benchmark: ";
        rules_list += (context->rules.benchmark.value() == Benchmark::NONE ? "None"
                            : context->rules.benchmark.value() == Benchmark::TIME ? "Time"
                            : context->rules.benchmark.value() == Benchmark::MEMORY ? "Memory"
                            : "All");

        std::cout << "Used Rules:\n\n" << rules_list << std::endl;
}

void Metaprocessor::list_variables() {
        if (context->variables.empty()) {
                std::cout << "No variables defined." << std::endl;
                return;
        }

        std::cout << "Defined variables: \n" << std::endl;
        for (const auto& var : context->variables) {
                if (var.second.size() == 1) {
                        std::cout << var.first << " = " << var.second[0] << std::endl;
                } else {
                        std::cout << var.first << " = [";
                        for (size_t i = 0; i < var.second.size(); ++i) {
                                std::cout << var.second[i];
                                if (i < var.second.size() - 1) {
                                        std::cout << ", ";
                                }
                        }
                        std::cout << "]" << std::endl;
                }
        }
}

}
