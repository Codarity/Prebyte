#include "PrebyteCLI.h"
#include "Prebyte.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        CLI cli = CLI::parse(argc, argv);

        Prebyte pre;
        for (const auto& [key, val] : cli.defines) {
            pre.define(key, val);
        }

        if (cli.use_stdin) {
            std::string result = pre.process_string(cli.input_string);
            std::cout << result << std::endl;
        } else {
            if (cli.output_path.empty()) {
                std::string result = pre.process_string(cli.input_path);
                std::cout << result << std::endl;
            } else {
                if (!pre.process_file(cli.input_path, cli.output_path)) {
                    return 1;
                }
            }
        }

        return 0;
    }
    catch (const std::exception& ex) {
        std::cerr << "[error] " << ex.what() << std::endl;
        return 1;
    }
}
