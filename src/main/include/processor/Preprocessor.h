#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "processor/Processor.h"
#include "processor/ProcessingVariables.h"

namespace prebyte {

class Preprocessor : public Processor {
private:
        std::string input;
        std::string output;
        ProcessingVariables process_variables;

        std::string get_input() const;
        void make_output() const;
        void process_all();
        void do_action(const std::string& action);
public:
        using Processor::Processor;
        Preprocessor(Context context) : Processor(context), process_variables(context) {}
        void process() override;
};

}
