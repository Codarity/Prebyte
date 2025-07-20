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
        std::map<std::string,std::string> variables;
        std::vector<std::string> inputs;
        std::unordered_set<std::string> ignore;
        std::map<std::string,Profile> profiles;
};

}
