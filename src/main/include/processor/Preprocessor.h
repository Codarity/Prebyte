#pragma once

#include "processor/Processor.h"

namespace prebyte {

class Preprocessor : public Processor {
public:
    using Processor::Processor;
    void process() override;
};

}
