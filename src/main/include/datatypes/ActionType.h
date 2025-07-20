#pragma once

namespace prebyte {

enum class ActionType {
        NONE,
        STDIN_STDOUT,
        FILE_IN_STDOUT,
        STDIN_FILE_OUT,
        FILE_IN_FILE_OUT,
        EXPLAIN,
        HELP,
        VERSION,
        LIST_RULES,
        LIST_VARIABLES,
        HARD_HELP
};

}
