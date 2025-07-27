#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stack>

#include "processor/Processor.h"
#include "processor/ProcessingVariables.h"
#include "processor/ProcessingFlow.h"
#include "datatypes/Context.h"
#include "processor/FlowState.h"
#include "parser/YamlParser.h"
#include "datatypes/Profile.h"
#include "parser/StringParser.h"
#include "datatypes/Rules.h"

namespace prebyte {

class Preprocessor : public Processor {
private:
        std::string input;
        std::string output;
        ProcessingVariables process_variables;
        ProcessingFlow process_flow;
        std::stack<FlowState> flow_states;
        std::vector<std::filesystem::path> processed_includes; 
        bool ignore_next = false;
        ushort current_depth = 0;
        ushort ignore_all = 0;
        ushort ignore_depth = 0;
        bool pipe = false;
        std::string profile_name;
        std::string profile_parser_type;
        std::string macro_name;
        std::string for_variable;
        int for_stack = 0;
        std::stack<std::vector<std::string>> macro_args;

        std::string get_input() const;
        void make_output() const;
        std::string process_all(std::string input);
        std::string do_action(const std::string& action);
        std::string process_code_flow(const std::string& action);
        void add_string(std::string& output, const std::string& str);
        void make_benchmark() const;

        std::map<std::string,std::string> get_rules(const Data& rules);
        std::unordered_set<std::string> get_ignore(const Data& ignore);
        std::map<std::string,std::vector<std::string>> get_variables(const Data& variables);
        std::vector<std::string> get_variable_values(std::string variable);

        std::string get_time_conversion(const std::chrono::nanoseconds& duration) const;
public:
        Preprocessor(std::unique_ptr<Context> context);
        void process() override;
};

}
