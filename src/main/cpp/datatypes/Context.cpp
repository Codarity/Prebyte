#include "datatypes/Context.h"

namespace prebyte {

void end(Context* context) {
        if (context->is_api) {
                throw std::runtime_error("There occurred an error while processing with Prebyte");
        } else {
                exit(1);
        }
}

}
