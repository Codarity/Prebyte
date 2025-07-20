#pragma once

#include <string>
#include <filesystem>
#include <fstream>

#include "datatypes/Context.h"

namespace prebyte {

class Processor {
protected:
    Context context;
    std::string get_variable(const std::string& variable_name) const;

public:
    explicit Processor(Context ctx) : context(std::move(ctx)) {}
    virtual ~Processor() = default;

    virtual void process() = 0;
};

}
