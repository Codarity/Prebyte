#pragma once

#include <chrono>
#include <string>
#include <vector>
#include "datatypes/ActionType.h"

namespace prebyte {
        
struct CliStruct {
        ActionType action;
        std::chrono::high_resolution_clock::time_point start_time;
        std::vector<std::string> input_args;
        std::vector<std::string> profiles;
        std::vector<std::string> variables; 
        std::vector<std::string> rules;
        std::vector<std::string> ignore;
        std::string log_level = "ERROR";
        std::string settings_file;
        std::string error_message;
        std::string warning_message;
};

}
