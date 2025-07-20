#pragma once

#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "processor/Processor.h"

namespace prebyte {

class Preprocessor : public Processor {
private:
        std::string input;
        std::string output;

        std::string get_input() const;
        void make_output() const;
        void process_all();
        void do_action(const std::string& action);
public:
        using Processor::Processor;
        void process() override;
};

}
