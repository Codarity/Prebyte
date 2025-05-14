#include "PrebyteCLI.h"
#include <iostream>
#include <stdexcept>

CLI CLI::parse(int argc, char** argv) {
    CLI cli;
    bool expect_string = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-s") {
            expect_string = true;
        }
        else if (arg.starts_with("-D")) {
            std::string def = arg.substr(2);
            auto eq = def.find('=');
            if (eq != std::string::npos) {
                std::string key = def.substr(0, eq);
                std::string val = def.substr(eq + 1);
                cli.defines[key] = val;
            } else {
                throw std::runtime_error("Invalid define: " + def);
            }
        }
        else if (expect_string) {
            cli.input_string = arg;
            cli.use_stdin = true;
            expect_string = false;
        }
        else if (cli.input_path.empty()) {
            cli.input_path = arg;
        }
        else if (cli.output_path.empty()) {
            cli.output_path = arg;
        }
    }

    // Minimalvalidierung
    if (!cli.use_stdin && cli.input_path.empty()) {
        throw std::runtime_error("No input file or -s string provided.");
    }

    return cli;
}
