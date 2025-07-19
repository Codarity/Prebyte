#pragma once

#include "datatypes/CliStruct.h"
#include <string>
#include <list>

namespace prebyte {

class CliParser {
private:
        std::string findAction(const std::string& arg);

public:
        CliStruct parse(std::list<std::string> args);
};

}
