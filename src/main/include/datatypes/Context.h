#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_set>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "datatypes/Rules.h"
#include "datatypes/ActionType.h"
#include "datatypes/Profile.h"

namespace prebyte {


struct Context {
        ActionType action_type;
        bool is_api = false;
        std::shared_ptr<spdlog::logger> logger;
        std::shared_ptr<spdlog::sinks::sink> console_sink;
        Rules rules;
        std::string input;
        std::string output;
        std::chrono::high_resolution_clock::time_point start_time;
        std::map<std::string,std::vector<std::string>> variables;
        std::vector<std::string> inputs;
        std::unordered_set<std::string> ignore;
        std::map<std::string,Profile> profiles;
        std::map<std::string,std::string> macros;
        int include_counter = 0;
};


void end(Context* context);

}
