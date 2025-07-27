#include "Executer.h"

namespace prebyte {

Executer::Executer(Context* context)
    : context(std::move(context)) {
}

void Executer::execute() {
        switch (context->action_type) {
                case ActionType::HELP:
                case ActionType::HARD_HELP:
                case ActionType::EXPLAIN:
                case ActionType::VERSION:
                case ActionType::LIST_RULES:
                case ActionType::LIST_VARIABLES: {
                        Metaprocessor processor(context);
                        processor.process();
                        break;
                }
                case ActionType::FILE_IN_FILE_OUT:
                case ActionType::FILE_IN_STDOUT:
                case ActionType::STDIN_FILE_OUT:
                case ActionType::STDIN_STDOUT: {
                        Preprocessor preprocessor(context);
                        preprocessor.process();
                        break;
                }
                default:
                        throw std::runtime_error("Unknown action type");
                        break;
        }
}

}
