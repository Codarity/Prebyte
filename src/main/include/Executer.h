#pragma once

#include <stdexcept>

#include "datatypes/Context.h"
#include "processor/Preprocessor.h"
#include "processor/Metaprocessor.h"

namespace prebyte {

class Executer {
private:
        Context context;

public:
        Executer(Context context);
        void execute();
};

}
