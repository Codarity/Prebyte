#pragma once

#include <vector>
#include <string>

#include "datatypes/Rules.h"
#include "datatypes/ActionType.h"
#include "datatypes/Variable.h"

namespace prebyte {

struct Context {
        ActionType action_type;
        Rules rules;
        std::vector<Variable> variables;
        std::vector<std::string> inputs;
};

}
