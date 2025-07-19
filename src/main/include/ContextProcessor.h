#pragma once

#include "datatypes/Context.h"
#include "datatypes/CliStruct.h"
#include "datatypes/ActionType.h"

namespace prebyte {

class ContextProcessor {
private:
        CliStruct cli_struct;
        Context context;
        ActionType action_type;

        void determine_action_type();
        void load_settings();
        void load_profiles();
        void load_variables();
        void load_ignore();
        void load_rules();
public:
        ContextProcessor(const CliStruct& cli_struct);
        Context process();
};

}
