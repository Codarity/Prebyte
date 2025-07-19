#pragma once

#include <string>
#include <vector>

namespace prebyte {
        
struct CliStruct {
        std::string action;
        std::vector<std::string> variables; 
        std::vector<std::string> rules;
        std::vector<std::string> settings;
        std::vector<std::string> ignore;
};

}
