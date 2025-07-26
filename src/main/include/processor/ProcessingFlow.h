#pragma once

#include <string>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <sstream>
#include <stack>
#include <regex>

#include "processor/FlowState.h"
#include "datatypes/Context.h"

namespace prebyte {

enum class FlowType {
        SET_VAR,
        UNSET_VAR,
        SET_RULE,
        DEFINE_PROFILE,
        SET_PROFILE,
        UNSET_PROFILE,
        SET_IGNORE,
        UNSET_IGNORE,
        DEFINE_MACRO,
        END_DEFINE,
        EXECUTE_MACRO,
        IF,
        ELSE_IF,
        ELSE,
        ENDIF,
        FOR,
        ENDFOR,
        INCLUDE
};

class ProcessingFlow {
private:
        Context* context;
        FlowState flow_state;

        std::string _SET_VAR(const std::string& action);
        std::string _UNSET_VAR(const std::string& action);
        std::string _SET_RULE(const std::string& action);
        std::string _DEFINE_PROFILE(const std::string& action);
        std::string _SET_PROFILE(const std::string& action);
        std::string _UNSET_PROFILE(const std::string& action);
        std::string _SET_IGNORE(const std::string& action);
        std::string _UNSET_IGNORE(const std::string& action);
        std::string _DEFINE_MACRO(const std::string& action);
        std::string _EXECUTE_MACRO(const std::string& action);
        std::string _IF(const std::string& action);
        std::string _ELSE_IF(const std::string& action);
        std::string _ELSE(const std::string& action);
        std::string _FOR(const std::string& action);
        std::string _INCLUDE(const std::string& action);

        bool is_true(const std::string& action) const;
        bool eval_or(const std::string& expr) const;
        bool eval_and(const std::string& expr) const;
        bool eval_not(const std::string& expr) const;
        bool eval_comparison(const std::string& expr) const;

        std::string get_str_value(const std::string& token) const;
        std::string trim(const std::string& s) const;


public:
        ProcessingFlow(Context* context) : context(context) {};

        bool is_valid(const std::string& action) const;
        std::string get_value(const std::string& action);
        FlowState get_flow_state() const;
        bool is_skipable(const std::string& action, bool ignore) const;
};

}
