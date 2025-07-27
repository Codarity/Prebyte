#include "Executer.h"

namespace prebyte {

Executer::Executer(std::unique_ptr<Context> context)
    : context(std::move(context)) {
}

void Executer::execute() {
        this->context->logger->debug("Executing action type: {}", static_cast<int>(context->action_type));
        switch (context->action_type) {
                case ActionType::HELP:
                case ActionType::HARD_HELP:
                case ActionType::EXPLAIN:
                case ActionType::VERSION:
                case ActionType::LIST_RULES:
                case ActionType::LIST_VARIABLES: {
                        Metaprocessor processor(std::move(context));
                        processor.process();
                        break;
                }
                case ActionType::FILE_IN_FILE_OUT:
                case ActionType::FILE_IN_STDOUT:
                case ActionType::STDIN_FILE_OUT:
                case ActionType::STDIN_STDOUT: {
                        Preprocessor preprocessor(std::move(context));
                        preprocessor.process();
                        break;
                }
                default:
                        this->context->logger->error("Unknown action type: {}", static_cast<int>(context->action_type));
                        this->context->logger->error("Please use --help to see available commands.");
                        end(context.get());
        }
}

}
