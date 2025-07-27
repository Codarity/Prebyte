#include "processor/Metaprocessor.h"

namespace prebyte {

Metaprocessor::Metaprocessor(std::unique_ptr<Context> context) : Processor() {
        this->context = std::move(context);
}

void Metaprocessor::process() {
        this->context->logger->debug("Starting metaprocessing...");
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
                this->context->logger->error("Unknown action type: {}", static_cast<int>(context->action_type));
                help();
                break;
        }
}

void Metaprocessor::explain() {
        std::string explanation;
        this->context->logger->debug("Executing explain command");
        
        if (context->inputs.empty()) {
                this->context->logger->debug("Explain command has no input");
                this->context->logger->debug("Showing general explanation");

                explanation = "Prebyte is a tool for processing files with various commands.\n"
                              "You can use it to read files, write output, and manage rules and variables.\n\n"
                              "If no input file is specified, prebyte will read from standard input and write to standard output.\n"
                              "If an input file is specified, prebyte will read from that file and write to standard output by default.\n";
                std::cout << explanation << std::endl;
                return;
        }
        std::string input = context->inputs[0];

        this->context->logger->debug("Input for explain command: {}", input);
        this->context->logger->debug("Writing explanation for input: {}", input);

        if (input == "ARGS") {
                explanation = "ARGS is a special Array in Prebyte that contains all the arguments passed to a macro.\n"
                              "It is used to access the arguments passed to a macro when it is called.\n"
                              "You can use ARGS to iterate over the arguments and perform operations on them.\n\n"
                              "Example usage:\n"
                              "  - ARGS[0] will give you the first argument passed to the macro.\n";
        } else if (input == "rules") {
                explanation = "Rules in Prebyte are used to define the behavior of the processor in various situations.\n"
                              "They can be used to control how variables are handled, how files are processed, and more.\n\n"
                              "You can list the used rules with the command: -lsr or --list-rules.\n"
                              "You can also define your own rules in the settings file.\n";
        } else if (input == "variables") {
                explanation = "Variables in Prebyte are used to store values that can be reused throughout the processing.\n"
                              "You can define variables in the settings file or pass them as command line arguments.\n\n"
                              "You can list the used variables with the command: -lsv or --list-variables.\n"
                              "Variables can be accessed using the variable prefix and suffix defined in the rules.\n";
        } else if (input == "settings") {
                explanation = "Settings in Prebyte are used to configure the behavior of the processor.\n"
                              "You can define settings in a settings file, which can be in JSON, YAML, or TOML format.\n\n"
                              "The settings file can include variables, rules, and other configurations that affect how Prebyte processes files.\n"
                              "You can store rules, variables, profiles and ignores in the settings file.\n"
                              "You can specify the settings file using the -s or --settings option. If not specified, Prebyte will try to load the system settings\n"
                              "The default settings file is located at ~/.prebyte/settings.json or ~/.prebyte/settings.yaml. If there is no settings file, prebyte loads its default values\n";
        } else if (input == "version") {
                explanation = "The version command displays the current version of Prebyte.\n"
                              "You can use it to check if you are using the latest version of the tool.\n\n"
                              "To see the version, use the command: -v or --version.";
        } else if (input == "explain") {
                explanation = "The explain command provides detailed information about how to use Prebyte and its commands.\n"
                              "You can use it to get explanations for specific commands or features.\n\n"
                              "To see the explanation, use the command: -e or --explain <command>.";
        } else if (input == "rule") {
                explanation = "A Rule in Prebyte is a single command or value that defines how the processor should behave in a specific situation.\n"
                              "Rules can be used to control how variables are handled, how files are processed, and more.\n\n"
                              "You can define rules in the settings file or pass them as command line arguments using the -r or --rule option.\n"
                              "Rules can be used to set default values for variables, control debugging levels, and more."
                              "The rules are: strict_variables, set_default_variables, trim_start, trim_end, allow_env, allow_env_fallback, debug_level, max_variable_length, default_variable_value, variable_prefix, variable_suffix, include_path, benchmark\n";

        } else if (input == "ignore") {
                explanation = "Ignore in Prebyte is a feature that allows you to exclude certain variables, even if they are defined in the settings file or passed as command line arguments.\n"
                              "You can define ignore patterns in the settings file or pass them as command line arguments using the -i or --ignore option.\n\n";
        } else if (input == "profile") {
                explanation = "A Profile in Prebyte is a set of rules, variables and ignores that can be applied to a specific context.\n"
                              "You can define profiles in the settings file, which can include variables, rules, and ignores.\n\n"
                              "Profiles allow you to apply different configurations based on the context in which Prebyte is used.\n"
                              "You can specify a profile using the -p or --profile option.";

        } else if (input == "strict_variables") {
                explanation = "The strict_variables rule ensures that all variables used in the processing\n"
                              "If strict_variables is disabled, it could be possible, that if a variable is not defined, it will be replaced with an empty string.\n"
                              "If strict_variables is enabled, it will throw an error if a variable is not defined.\n";
        } else if (input == "set_default_variables") {
                explanation = "The set_default_variables rule allows you to set default values for variables that are not defined.\n"
                              "If set_default_variables is enabled, it will replace undefined variables with the default_variable_value specified in the rules.\n"
                              "If set_default_variables is enabled, strict_variables will be ignored, because every variable will be replaced with a value.\n";
        } else if (input == "trim_start") {
                explanation = "The trim_start rule removes leading whitespace from the variable values.\n"
                              "If trim_start is enabled, it will remove any leading whitespace from the variable values before processing them.\n"
                              "This can be useful to ensure that variable values are clean and do not contain unnecessary whitespace at the beginning.";
        } else if (input == "trim_end") {
                explanation = "The trim_end rule removes trailing whitespace from the variable values.\n"
                              "If trim_end is enabled, it will remove any trailing whitespace from the variable values before processing them.\n"
                              "This can be useful to ensure that variable values are clean and do not contain unnecessary whitespace at the end.";
        } else if (input == "allow_env") {
                explanation = "The allow_env rule allows you to use environment variables in your Prebyte processing.\n"
                              "If allow_env is enabled, you can access environment variables using the variable prefix and suffix defined in the rules and also an $ before the environment variable that should be accessed.\n"
                              "This can be useful to access system-level variables or configuration values that are set in the environment.\n"
                              "If allow_env is disabled, environment variables will not be accessible in the processing.";
        } else if (input == "allow_env_fallback") {
                explanation = "The allow_env_fallback rule allows you to use environment variables as a fallback if a variable is not defined.\n"
                              "If allow_env_fallback is enabled, it will first check if the variable is defined in the context, and if not, it will try to access the environment variable with the same name.\n"
                              "This can be useful to provide default values for variables that are not defined in the context but may be set in the environment.";
        } else if (input == "debug_level") {
                explanation = "The debug_level rule allows you to set the level of debugging information that Prebyte will output during processing.\n"
                              "The available debug levels are ERROR, WARNING, INFO, and DEBUG.\n"
                              "If debug_level is set to ERROR, only error messages will be displayed.\n"
                              "If set to WARNING, both error and warning messages will be displayed.\n"
                              "If set to INFO, it will display error, warning, and informational messages.\n"
                              "If set to DEBUG, it will display all messages including debug information.";
        } else if (input == "max_variable_length") {
                explanation = "The max_variable_length rule sets a limit on the length of variable values.\n"
                              "If max_variable_length is set to a positive integer, it will cut off variable values that exceed this length.\n"
                              "If set to -1, there is no limit on the length of variable values.\n"
                              "This can be useful to prevent excessively long variable values that could cause performance issues or errors during processing.";
        } else if (input == "default_variable_value") {
                explanation = "The default_variable_value rule sets a default value for variables that are not defined in the context.\n"
                              "If set_default_variables is enabled, this value will be used to replace undefined variables.\n"
                              "This can be useful to provide a fallback value for variables that are not defined, ensuring that processing can continue without errors.";
        } else if (input == "variable_prefix") {
                explanation = "The variable_prefix rule sets the prefix used to identify variables in Prebyte.\n"
                              "By default, the prefix is set to %%, meaning that variables are accessed using the syntax %%variable_name.\n"
                              "You can change the variable prefix to any string you prefer, allowing you to customize how variables are accessed in your processing.\n"
                              "For example, if the prefix is set to 'var_', you would access a variable named 'example' as var_example%% because the suffix is also %%.\n";
        } else if (input == "variable_suffix") {
                explanation = "The variable_suffix rule sets the suffix used to identify variables in Prebyte.\n"
                              "By default, the suffix is set to %%.\n"
                              "You can change the variable suffix to any string you prefer, allowing you to customize how variables are accessed in your processing.\n"
                              "For example, if the suffix is set to '_var', you would access a variable named 'example' as %%example_var.";
        } else if (input == "include_path") {
                explanation = "The include_path sets the path where Prebyte will look for unfound include Files.\n"
                              "This can be useful to specify a directory where additional files are stored to be included by calling the include command.\n"
                              "You can set the include_path in the rules, and it will be used to resolve include files that are not found in the current working directory or full path.\n"
                              "By default, the include_path is set to ~/.prebyte/includes. This is recommended to be used for user-specific includes, because in future support a package manager to install usefull include files like an include where an macro is defined and includes all files given to the macro arguments.\n";
        } else if (input == "benchmark") {
                explanation = "The benchmark rule allows you to enable benchmarking features in Prebyte.\n"
                              "You can choose to benchmark time, memory, or both during processing.\n"
                              "This can be useful to analyze the performance of your Prebyte scripts and identify potential bottlenecks.\n"
                              "The available options are: NONE, TIME, MEMORY, ALL.";
        } else if (input == "help") {
                explanation = "The help command provides information about how to use Prebyte and its commands.\n"
                              "You can use it to get a list of available commands and their usage.\n\n"
                              "To see the help message, use the command: -h or --help.";
        } else if (input == "condition") {
                explanation = "Conditions in Prebyte are used to control the flow of processing based on certain criteria.\n"
                              "You can use conditions to check if a variable is defined, if a file exists, or any other condition that can be evaluated.\n\n"
                              "Conditions can be used in macros to control which parts of the macro are executed based on the evaluation of the condition.\n"
                              "You can define conditions using the syntax: if <condition>"
                              "A condition compares two values. Either a variable or a String Value.\n"
                              "You can use the following operators: ==, !=.\n"
                              "For example, you can use the condition: %%if VARIABLE_1 == VARIABLE_2%% to check if VARIABLE_1 is equal to VARIABLE_2.\n"
                              "Also you can use || or && to combine multiple conditions.\n";
        } else if (input == "set") {
                explanation = "The set command in Prebyte is used to define or modify variables. But also to set a profile, ignore or rule.\n"
                              "You start with your prefix and write 'set' followed by the specific command.\n"
                              "For variables, you type 'var', for profiles 'profile', for ignores 'ignore' or 'igno' and for rules 'rule'.\n"
                              "For example, to set a variable, you would use: %%set var VariableName=\"Value\"%%\n"
                              "If you set a variable without a value, it will be set to an empty string by default.\n";
        } else if (input == "unset") {
                explanation = "The unset command in Prebyte is used to remove variables or ignores that have been previously defined.\n"
                              "You start with your prefix and write 'unset' followed by the specific command.\n"
                              "For variables, you type 'var', for ignores 'ignore' or 'igno'.\n"
                              "For example, to unset a variable, you would use: %%unset var VariableName%%\n"
                              "This will remove the variable from the context, and it will no longer be accessible in the processing.";
        } else if (input == "define") {
                explanation = "The define command in Prebyte is used to create macros or profiles.\n"
                              "You start with your prefix and write 'define' followed by the specific command.\n"
                              "For macros, you type 'macro', for profiles 'profile'.\n"
                              "For example, to define a macro, you would use: %%define macro MacroName%%\n"
                              "This will create a new macro that can be called later in the processing.\n"
                              "You can also define a profile using the same syntax."
                              "The body of the macro or profile are the following lines until an 'enddef' command encounters.\n"
                              "An full example of a macro definition would be:\n"
                              "%%define macro my_macro%%\n"
                              "Here are my macro contents\n"
                              "%%enddef%%\n"
                              "Profiles have an additional step. Because an Profile is a set of rules, variables and ignores that can be applied to a specific context.\n"
                              "By default, the body of the profile is parsed as YAML, but you can specify a different parser type by adding it after the profile name.\n"
                              "For example, to define a profile with YAML parser, you would use: %%define profile my_profile yaml%%\n"
                              "Supported parsers are: yaml, json, toml.\n";
        } else if (input == "include") {
                explanation = "The include command in Prebyte is used to include external files into the processing.\n"
                              "You start with your prefix and write 'include' followed by the file path.\n"
                              "For example, to include a file, you would use: %%include path/to/file%%\n"
                              "This will read the contents of the specified file and include it in the processing on the current position.\n"
                              "If no path is specified, it will look for the file in the current working directory.\n"
                              "If the file is not found in the current working directory, it will look for it in the include_path specified in the rules.\n"
                              "You can add additional Files in the default include path, so you can reuse them.\n";
        } else if (input == "for" || input == "foreach" || input == "loop") {
                explanation = "The for command in Prebyte is used to iterate over a collection of items, such as an array or a list of variables.\n"
                              "You start with your prefix and write 'for' followed by the variable name and the collection to iterate over.\n"
                              "For example, to iterate over an array, you would use: %%for item in my_array%%\n"
                              "This will execute the body of the loop for each item in the collection, allowing you to perform operations on each item.\n"
                              "To end the loop, you use the 'endfor' command: %%endfor%%. This will mark the end of the loop body, not as an break statement.\n"
                              "This for loop will copy the array, to ensure that the original array is not modified during the loop.\n"
                              "The loop variable is reassigned during each iteration and remains defined after the loop.\n";
        } else if (input == "endfor") {
                explanation = "The endfor command in Prebyte is used to mark the end of a for loop.\n"
                              "You start with your prefix and write 'endfor'.\n"
                              "For example, to end a for loop, you would use: %%endfor%%\n"
                              "This will mark the end of the loop body, allowing you to continue processing after the loop.\n"
                              "It is important to ensure that every for loop has a corresponding endfor command to avoid syntax errors.";
        } else if (input == "if" || input == "condition") {
                explanation = "The if command in Prebyte is used to create conditional statements that control the flow of processing based on certain criteria.\n"
                              "You start with your prefix and write 'if' followed by the condition to evaluate.\n"
                              "For example, to check if a variable is defined, you would use: %%if VARIABLE_NAME%%\n"
                              "This will execute the body of the if statement if the condition is true.\n"
                              "To end the if statement, you use the 'endif' command: %%endif%%. This will mark the end of the if body.\n"
                              "You can also use 'else' to provide an alternative block of code that will be executed if the condition is false: %%else%%\n"
                              "You can combine multiple conditions using '&&' (and) or '||' (or) operators.\n"
                              "There are also 'elif' and 'else' commands to provide additional conditions or alternative blocks.\n";
        } else if (input == "endif") {
                explanation = "The endif command in Prebyte is used to mark the end of an if statement.\n"
                              "You start with your prefix and write 'endif'.\n"
                              "For example, to end an if statement, you would use: %%endif%%\n"
                              "This will mark the end of the if body, allowing you to continue processing after the if statement.\n"
                              "It is important to ensure that every if statement has a corresponding endif command to avoid syntax errors.";
        } else if (input == "else") {
                explanation = "The else command in Prebyte is used to provide an alternative block of code that will be executed if the condition in an if statement is false.\n"
                              "You start with your prefix and write 'else'.\n"
                              "For example, to provide an alternative block, you would use: %%else%%\n"
                              "This will execute the body of the else statement if the condition in the preceding if statement is false.";
        } else if (input == "elif") {
                explanation = "The elif command in Prebyte is used to provide an additional condition in an if statement.\n"
                              "You start with your prefix and write 'elif' followed by the condition to evaluate.\n"
                              "For example, to check another condition, you would use: %%elif VARIABLE_NAME == 'value'%%\n"
                              "This will execute the body of the elif statement if the preceding if statement's condition is false and the elif condition is true.\n"
                              "You can have multiple elif statements in a single if block.";
        } else if (input == "macro") {
                explanation = "A macro in Prebyte is a reusable block of code that can be defined and called later in the processing.\n"
                              "You start with your prefix and write 'macro' followed by the macro name.\n"
                              "For example, to define a macro, you would use: %%define macro MacroName%%\n"
                              "The body of the macro is defined in the following lines until an 'enddef' command encounters.\n"
                              "Macros can accept arguments, which are passed as an array called ARGS when the macro is called.\n"
                              "You can call a macro using: %%exec MacroName \"First Argument\" Variable_As_Argument%%.";
        } else if (input == "exec") {
                explanation = "The exec command in Prebyte is used to call a previously defined macro.\n"
                              "You start with your prefix and write 'exec' followed by the macro name and any arguments you want to pass.\n"
                              "For example, to call a macro, you would use: %%exec MacroName \"First Argument\" Variable_As_Argument%%\n"
                              "This will execute the body of the macro with the provided arguments, which are accessible as an array called ARGS within the macro.\n"
                              "You can pass multiple arguments separated by spaces.";
        } else if (input == "lsr") {
                explanation = "The lsr command in Prebyte is used to list the rules that are currently in use.\n"
                              "You start with your prefix and write '-lsr'.\n"
                              "For example, to list the rules, you would use: %%lsr%%\n"
                              "This will output a list of all the rules that are currently defined and their values.\n"
                              "This can be useful to see which rules are active and how they are configured.";
        } else if (input == "lsv") {
                explanation = "The lsv command in Prebyte is used to list the variables that are currently defined.\n"
                              "You start with your prefix and write '-lsv'.\n"
                              "For example, to list the variables, you would use: %%lsv%%\n"
                              "This will output a list of all the variables that are currently defined and their values.\n"
                              "This can be useful to see which variables are available for use in the processing.";
        } else if (input == "input") {
                explanation = "The input in Prebyte is either a file or standard input that Prebyte reads from.\n"
                              "If no input file is specified, Prebyte will read from standard input (stdin).\n"
                              "If an input file is specified, Prebyte will read from that file.\n"
                              "If you want to read from a specific file, you can use: prebyte path/to/file.txt or prebyte file.txt\n";
        } else if (input == "output") {
                explanation = "The output in Prebyte is where the processed data is written to.\n"
                              "By default, if no output file is specified, Prebyte will write to standard output (stdout).\n"
                              "You can specify an output file using the -o or --output option followed by the file path.\n"
                              "For example, to write the output to a file, you would use: prebyte -o path/to/output.txt or prebyte --output output.txt\n"
                              "If an input file is specified, Prebyte will read from that file and write to standard output by default.";
        } else {
                this->context->logger->error("Unknown input for explain command: {}", input);
                explanation = "Please refer to the help command for more information.";
        }

        std::cout << explanation << std::endl;
}

void Metaprocessor::help() {
        this->context->logger->debug("Showing help message");

std::cout << "   Usage: prebyte <command> [options]\n\n"
         << "      Prebyte is a tool for processing files with various commands.\n"
         << "      You can use it to read files, write output, and manage rules and variables.\n\n"
         << "      If no input file is specified, prebyte will read from standard input and write to standard output.\n"
         << "      If an input file is specified, prebyte will read from that file and write to standard output by default.\n\n"

         << "   Available commands:\n"
         << "\t<file>                  Process the specified file\n"
         << "\t-o, --output <file>     Write output to the specified file\n"
         << "\t<file> -o <file>        Read from the specified file and write output to another file\n"
         << "\t-h, --help              Show this help message\n"
         << "\t-v, --version           Show version information\n"
         << "\t-e, --explain           Explain the usage of the tool or a specific command\n"
         << "\t-lsr, --list-rules      List used rules\n"
         << "\t-lsv, --list-variables  List used variables\n"
         << "\t-s, --settings <file>   Specify a settings file to use\n"
         << "\n"
         << "   Available options:\n"
         << "\t-r, --rule <rule>       Set a rule to be used during processing. <rule> is an KEY=VALUE pair\n"
         << "\t-i, --ignore <pattern>  Ignore variables that match the specified pattern\n"
         << "\t-P<name>                Use a specific profile for processing. The Profile must be defined before preprocessing\n"
         << "\t-p, --profile <name>    Use a specific profile for processing. The Profile must be defined before preprocessing\n"
         << "\t-D<key>=<value>         Define a variable with the specified key and value\n"
         << "\t-d, --define            Define a variable with the specified key and value\n"
         << "\t   <key>=<value>\n"
         << "\t--<debug_level>          Set the debug level for processing. Available levels: error, warning, info, debug, trace, off\n"
         << "\n"
         << "   Available rules:\n"
         << "\tstrict_variables        Ensure all variables are defined\n"
         << "\tset_default_variables   Set default values for undefined variables\n"
         << "\ttrim_start              Remove leading whitespace from variable values\n"
         << "\ttrim_end                Remove trailing whitespace from variable values\n"
         << "\tallow_env               Allow access to environment variables\n"
         << "\tallow_env_fallback      Allow environment variables as fallback for undefined variables\n"
         << "\tdebug_level             Set the level of debugging information (ERROR, WARNING, INFO, DEBUG)\n"
         << "\tmax_variable_length     Set a limit on the length of variable values\n"
         << "\tdefault_variable_value  Set a default value for undefined variables\n"
         << "\tvariable_prefix         Set the prefix used to identify variables\n"
         << "\tvariable_suffix         Set the suffix used to identify variables\n"
         << "\tinclude_path            Set the path where Prebyte will look for include files\n"
         << "\tbenchmark               Enable benchmarking features (NONE, TIME, MEMORY, ALL)\n";
}

void Metaprocessor::hard_help() {
        this->context->logger->debug("Showing hard help message, because no matching command or options were provided");
        this->context->logger->error("Invalid command or options provided.\n");
        end(this->context.get());
}

void Metaprocessor::version() {
        this->context->logger->debug("Showing version information");
        std::cout << "Prebyte Version: " << VERSION << std::endl;
}

void Metaprocessor::list_rules() {
        this->context->logger->debug("Listing used rules");
        std::string rules_list;
        rules_list += "strict_variables: " + std::string(context->rules.strict_variables.value() ? "true" : "false") + "\n";
        rules_list += "set_default_variables: " + std::string(context->rules.set_default_variables.value() ? "true" : "false") + "\n";
        rules_list += "trim_start: " + std::string(context->rules.trim_start.value() ? "true" : "false") + "\n";
        rules_list += "trim_end: " + std::string(context->rules.trim_end.value() ? "true" : "false") + "\n";
        rules_list += "allow_env: " + std::string(context->rules.allow_env.value() ? "true" : "false") + "\n";
        rules_list += "allow_env_fallback: " + std::string(context->rules.allow_env_fallback.value() ? "true" : "false") + "\n";
        rules_list += "debug_level: ";
        rules_list +=
            (context->rules.debug_level.value() == spdlog::level::err   ? "[ERROR] "
             : context->rules.debug_level.value() == spdlog::level::warn ? "[WARNING] "
             : context->rules.debug_level.value() == spdlog::level::info    ? "[INFO] "
             : context->rules.debug_level.value() == spdlog::level::debug ? "[DEBUG] "
             : context->rules.debug_level.value() == spdlog::level::trace ? "[TRACE] "
             : context->rules.debug_level.value() == spdlog::level::off   ? "[OFF] "
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

        std::string rules_debug_list = "Used Rules:  " + rules_list;
        std::replace(rules_debug_list.begin(), rules_debug_list.end(), '\n', ' ');
        this->context->logger->debug("Used Rules: {}", rules_debug_list);

        std::cout << "Used Rules:\n\n" << rules_list << std::endl;
}

void Metaprocessor::list_variables() {
        this->context->logger->debug("Listing used variables");
        if (context->variables.empty()) {
                this->context->logger->warn("No variables defined");
                return;
        }

        this->context->logger->debug("Listing defined variables");
        std::cout << "Defined variables: \n" << std::endl;
        std::string found_variables = "";
        for (const auto& var : context->variables) {
                if (var.second.size() == 1) {
                        context->logger->trace("Variable {} has a single value: {}", var.first, var.second[0]);
                        found_variables += var.first + "=" + var.second[0] + "  ";
                        std::cout << var.first << " = " << var.second[0] << std::endl;
                } else {
                        context->logger->trace("Variable {} has multiple values", var.first);
                        found_variables += var.first + "=[";
                        std::cout << var.first << "=[";
                        for (size_t i = 0; i < var.second.size(); ++i) {
                                std::cout << var.second[i];
                                if (i < var.second.size() - 1) {
                                found_variables += var.second[i] + ",";
                                        std::cout << ", ";
                                }
                        }
                        found_variables += "]";
                        std::cout << "]" << std::endl;
                }
        }
        this->context->logger->debug("Found variables: {}", found_variables);
}

}
