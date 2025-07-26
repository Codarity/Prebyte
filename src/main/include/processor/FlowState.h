#pragma once

namespace prebyte {

enum class FlowState {
        NONE,
        EXECUTE_MACRO,
        DEFINE_PROFILE,
        DEFINE_MACRO,
        END_DEFINE,
        IF,
        ELSE_IF,
        ELSE,
        END_IF,
        FOR,
        END_FOR,
        INCLUDE,
};

}
