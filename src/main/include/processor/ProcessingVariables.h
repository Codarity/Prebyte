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

        std::string _DATE() const;
        std::string _TIME() const;
        std::string _LINE() const;
        std::string _DATETIME() const;
        std::string _VERSION() const;
        std::string _YEAR() const;
        std::string _MONTH() const;
        std::string _DAY() const;
        std::string _HOUR() const;
        std::string _MINUTE() const;
        std::string _SECOND() const;
        std::string _UNIXTIMESTAMP() const;
        std::string _USER() const;
        std::string _HOST() const;
        std::string _PWD() const;
        std::string _FILE() const;
        std::string _FILE_NAME() const;
        std::string _FILE_PATH() const;
        std::string _FILE_EXT() const;
        std::string _FILE_SIZE() const;
        std::string _FILE_CREATED() const;


public:
        ProcessingVariables(Context* context);
        bool is_valid(const std::string& key) const;
        std::string get_value(const std::string& key) const;
private:

};

}
