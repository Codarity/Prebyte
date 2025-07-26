#pragma once

#include <string>
#include <vector>
#include "datatypes/ActionType.h"

namespace prebyte {
        
struct CliStruct {
        ActionType action;
        std::vector<std::string> input_args;
        std::vector<std::string> profiles;
        std::vector<std::string> variables; 
        std::vector<std::string> rules;
        std::vector<std::string> ignore;
        std::string settings_file;
        std::string error_message;
        std::string warning_message;
};

}
