#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>


#include "datatypes/Context.h"

namespace prebyte {

class Processor {
protected:
        Context* context;
        std::string get_variable(const std::string& variable_name) const;
        std::string get_variable_name(const std::string& action) const;
        std::string get_variable_value(const std::string& action, bool pattern) const;
        int get_variable_number(const std::string& action) const;

public:
    virtual ~Processor() = default;

    virtual void process() = 0;
};

}
