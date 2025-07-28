#include "parser/CliParser.h"

namespace prebyte {

CliStruct CliParser::parse(std::list<std::string> args) {
        this->cli_struct.start_time = std::chrono::high_resolution_clock::now();
        this->cli_struct.action = this->findAction(&args);
        this->process(std::vector<std::string>(args.begin(), args.end()));
        return this->cli_struct;
}

ActionType CliParser::findAction(std::list<std::string>* args) {
        if(args->empty()) return ActionType::STDIN_STDOUT;

        if(args->front() == "-o" || args->front() == "--output") {
                args->pop_front();
                if(args->empty()) return ActionType::HARD_HELP;

                cli_struct.input_args.push_back(args->front());
                args->pop_front();
                return ActionType::STDIN_FILE_OUT;
        }

        if(args->front().starts_with("-")) {
                ActionType action_type = ActionType::NONE;
                if(args->front() == "-h" || args->front() == "--help") {
                        action_type = ActionType::HELP;
                } else if(args->front() == "-v" || args->front() == "--version") {
                        action_type = ActionType::VERSION;
                } else if(args->front() == "-e" || args->front() == "--explain") {
                        action_type = ActionType::EXPLAIN;
                } else if(args->front() == "-lsr" || args->front() == "--list-rules") {
                        action_type = ActionType::LIST_RULES;
                } else if(args->front() == "-lsv" || args->front() == "--list-variables") {
                        action_type = ActionType::LIST_VARIABLES;
                }

                if(action_type != ActionType::NONE) {
                        args->pop_front();
                        std::copy(args->begin(), args->end(), std::back_inserter(cli_struct.input_args));
                        return action_type;
                }
        }

        if(std::filesystem::exists(args->front())) {
                cli_struct.input_args.push_back(args->front());
                args->pop_front();

                if(args->empty()) return ActionType::FILE_IN_STDOUT;

                if(args->front() == "-o" || args->front() == "--output") {
                        args->pop_front();
                        if(args->empty()) return ActionType::HARD_HELP;

                        cli_struct.input_args.push_back(args->front());
                        args->pop_front();
                        return ActionType::FILE_IN_FILE_OUT;
                }

                return ActionType::FILE_IN_STDOUT;
        }

        if(args->front().starts_with("-D") || args->front() == "-r" || args->front() == "--rule" ||
           args->front() == "-i" || args->front() == "--ignore" || args->front() == "-p" || args->front() == "--profile" ||
           args->front().starts_with("-P") || args->front() == "-s" || args->front() == "--settings") {
                return ActionType::STDIN_STDOUT;
        }

        return ActionType::HARD_HELP;
}

void CliParser::process(const std::vector<std::string>& args) {
        for(int i = 0; i < args.size(); i++) {
                const std::string& arg = args[i];
                if(arg == "-r" || arg == "--rule") {
                        if(i + 1 < args.size()) {
                        this->cli_struct.rules.push_back(args[++i]);
                        } else {
                                throw std::runtime_error("Missing rule after " + arg);
                        }
                } else if(arg == "-s" || arg == "--settings") {
                        if(i + 1 < args.size()) {
                                this->cli_struct.settings_file = args[++i];
                        } else {
                                throw std::runtime_error("Missing settings file after " + arg);
                        }
                } else if(arg == "-i" || arg == "--ignore") {
                        if(i + 1 < args.size()) {
                                this->cli_struct.ignore.push_back(args[++i]);
                        } else {
                                throw std::runtime_error("Missing ignore after " + arg);
                        }
                } else if(arg == "-p" || arg == "--profile") {
                        if(i + 1 < args.size()) {
                                this->cli_struct.profiles.push_back(args[++i]);
                        } else {
                                throw std::runtime_error("Missing profile after " + arg);
                        }
                } else if(arg.starts_with("-P")) {
                        if(arg == "-P") throw std::runtime_error("Missing profile path after -P");
                        this->cli_struct.profiles.push_back(arg.substr(2));
                } else if(arg == "-d" || arg == "--define") {
                        if(i + 1 < args.size()) {
                                this->cli_struct.variables.push_back(args[++i]);
                        } else {
                                throw std::runtime_error("Missing variable definition after " + arg);
                        }
                } else if(arg.starts_with("-D")) {
                        if(arg == "-D") throw std::runtime_error("Missing variable definition after -D");
                        this->cli_struct.variables.push_back(arg.substr(2));
                } else if (arg == "--trace") {
                        this->cli_struct.log_level = "TRACE";
                } else if (arg == "--debug" || arg == "-X") {
                        this->cli_struct.log_level = "DEBUG";
                } else if (arg == "--info") {
                        this->cli_struct.log_level = "INFO";
                } else if (arg == "--warn" || arg == "--warning") {
                        this->cli_struct.log_level = "WARN";
                } else if (arg == "--error" || arg == "--err") {
                        this->cli_struct.log_level = "ERROR";
                } else if (arg == "--off") {
                        this->cli_struct.log_level = "OFF";
                } else {
                        this->cli_struct.warning_message = "Unknown argument: " + arg;
                }

        }
}


}
