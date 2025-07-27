#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_set>

#include "datatypes/Rules.h"
#include "datatypes/ActionType.h"
#include "datatypes/Profile.h"

namespace prebyte {

struct Context {
        ActionType action_type;
        Rules rules;
        std::string input;
        std::string output;
        std::chrono::high_resolution_clock::time_point start_time;
        std::map<std::string,std::vector<std::string>> variables;
        std::vector<std::string> inputs;
        std::unordered_set<std::string> ignore;
        std::map<std::string,Profile> profiles;
        std::map<std::string,std::string> macros;
        int include_counter = 0;
};

}
