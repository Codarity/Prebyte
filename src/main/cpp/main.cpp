#include <list>
#include <string>
#include <iostream>
#include "parser/CliParser.h"
#include "datatypes/CliStruct.h"
#include "datatypes/Context.h"
#include "ContextProcessor.h"
#include "Executer.h"


int main(int argc, char** argv) {
        try {
                if (argc < 2) {
                        prebyte::Context context;
                        context.action_type = prebyte::ActionType::HELP;
                        prebyte::Executer executer(context); 
                        executer.execute();
                        return 1;
                }
                std::list<std::string> args;
                for(int i = 1; i < argc; ++i) {
                        args.push_back(argv[i]);
                } 

                prebyte::CliParser cli_parser;
                prebyte::CliStruct cli_struct = cli_parser.parse(args);
                prebyte::ContextProcessor context_processor(cli_struct);
                prebyte::Context context = context_processor.process();
                prebyte::Executer executer(context);
                executer.execute();
        } catch (...) {
                return 1;
        }
        return 0;
}
