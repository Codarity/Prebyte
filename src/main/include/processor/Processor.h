#pragma once

#include "datatypes/Context.h"

namespace prebyte {

class Processor {
protected:
    Context context;

public:
    explicit Processor(Context ctx) : context(std::move(ctx)) {}
    virtual ~Processor() = default;

    virtual void process() = 0;
};

}
