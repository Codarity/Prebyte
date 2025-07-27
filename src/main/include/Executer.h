#pragma once

#include <stdexcept>

#include "datatypes/Context.h"
#include "processor/Preprocessor.h"
#include "processor/Metaprocessor.h"

namespace prebyte {

class Executer {
private:
        std::unique_ptr<Context> context;

public:
        Executer(std::unique_ptr<Context> context);
        void execute();
};

}
