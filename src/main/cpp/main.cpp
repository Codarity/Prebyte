#include <list>
#include <string>
#include "parser/CliParser.h"
#include "datatypes/CliStruct.h"
#include "datatypes/Context.h"
#include "ContextProcessor.h"
#include "Executer.h"


int main(int argc, char** argv) {
        try {
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
        } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
        }
        return 0;
}
