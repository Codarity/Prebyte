#pragma once

#include <string>
#include <iostream>

#include "processor/Processor.h"


extern const std::string VERSION;

namespace prebyte {

class Metaprocessor : public Processor {
private:
        void explain();
        void help();
        void hard_help();
        void version();
        void list_rules();
        void list_variables();
public:
        Metaprocessor(Context* context);

        void process() override;
};

}
