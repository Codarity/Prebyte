#pragma once

#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <unordered_map>
#include <random>
#include <format>
#include <functional>

#include "datatypes/Context.h"

extern const std::string VERSION;

namespace prebyte {



enum class VariableAction {
        DATE,
        TIME,
        DATETIME,
        LINE,
        VERSION,
        YEAR,
        MONTH,
        DAY,
        HOUR,
        MINUTE,
        SECOND,
        UNIXTIMESTAMP,
        USER,
        HOST,
        PWD,
        FILE,
        FILE_NAME,
        FILE_PATH,
        FILE_EXT,
        FILE_SIZE,
        FILE_CREATED,
};

class ProcessingVariables {
private:

        std::unordered_map<std::string,std::function<std::string()>> functionality;
        std::chrono::time_point<std::chrono::system_clock> current_time;
        Context context;


        std::tm getLocalTime() const;

        std::string __DATE() const;
        std::string __TIME() const;
        std::string __LINE() const;
        std::string __DATETIME() const;
        std::string __VERSION() const;
        std::string __YEAR() const;
        std::string __MONTH() const;
        std::string __DAY() const;
        std::string __HOUR() const;
        std::string __MINUTE() const;
        std::string __SECOND() const;
        std::string __UNIXTIMESTAMP() const;
        std::string __USER() const;
        std::string __HOST() const;
        std::string __PWD() const;
        std::string __FILE() const;
        std::string __FILE_NAME() const;
        std::string __FILE_PATH() const;
        std::string __FILE_EXT() const;
        std::string __FILE_SIZE() const;
        std::string __FILE_CREATED() const;


public:
        ProcessingVariables(Context context);
        bool is_valid(const std::string& key) const;
        std::string get_value(const std::string& key) const;
private:

};



}
