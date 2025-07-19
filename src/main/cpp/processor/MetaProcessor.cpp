#include "processor/Metaprocessor.h"

namespace prebyte {

void Metaprocessor::process() {
        switch (context.action_type) {
        case ActionType::EXPLAIN:
                explain();
                break;
        case ActionType::HELP:
                help();
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
                std::cerr << "Unknown action type: " << static_cast<int>(context.action_type) << std::endl;
                help();
                break;
        }
}

void Metaprocessor::explain() {
}

void Metaprocessor::help() {
        std::cout << "Usage: prebyte <command> [options]\n";
}

void Metaprocessor::version() {
}

void Metaprocessor::list_rules() {
}

void Metaprocessor::list_variables() {
}

}
