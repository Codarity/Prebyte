#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <list>
#include <spdlog/spdlog.h>

#include "datatypes/CliStruct.h"

namespace prebyte {

class CliParser {
private:
        CliStruct cli_struct;
        ActionType findAction(std::list<std::string>* args);
        void process(const std::vector<std::string>& args);

public:
        CliStruct parse(std::list<std::string> args);
};

}
