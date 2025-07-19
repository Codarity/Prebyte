#pragma once

#include <string>
#include <iostream>

#include "processor/Processor.h"

namespace prebyte {

class Metaprocessor : public Processor {
private:
        void explain();
        void help();
        void version();
        void list_rules();
        void list_variables();
public:
    using Processor::Processor;
    void process() override;
};

}
