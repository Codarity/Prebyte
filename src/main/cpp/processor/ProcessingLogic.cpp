#include "processor/ProcessingVariables.h"

namespace prebyte {

ProcessingVariables::ProcessingVariables(Context context) : context(context), current_time(std::chrono::system_clock::now()) {
        functionality = {
        { "__DATE__",          [this]() { return this->__DATE(); } },
        { "__TIME__",          [this]() { return this->__TIME(); } },
        { "__DATETIME__",      [this]() { return this->__DATETIME(); } },
        { "__YEAR__",          [this]() { return this->__YEAR(); } },
        { "__MONTH__",         [this]() { return this->__MONTH(); } },
        { "__DAY__",           [this]() { return this->__DAY(); } },
        { "__HOUR__",          [this]() { return this->__HOUR(); } },
        { "__MINUTE__",        [this]() { return this->__MINUTE(); } },
        { "__SECOND__",        [this]() { return this->__SECOND(); } },
        { "__UNIXTIMESTAMP__", [this]() { return this->__UNIXTIMESTAMP(); } },
        { "__USER__",          [this]() { return this->__USER(); } },
        { "__HOST__",          [this]() { return this->__HOST(); } },
        { "__PWD__",           [this]() { return this->__PWD(); } },
        { "__VERSION__",       [this]() { return this->__VERSION(); } },
        { "__FILE__",          [this]() { return this->__FILE(); } },
        { "__FILE_NAME__",     [this]() { return this->__FILE_NAME(); } },
        { "__FILE_PATH__",     [this]() { return this->__FILE_PATH(); } },
        { "__FILE_EXT__",      [this]() { return this->__FILE_EXT(); } },
        { "__FILE_SIZE__",     [this]() { return this->__FILE_SIZE(); } },
        { "__FILE_CREATED__",  [this]() { return this->__FILE_CREATED(); } },
        { "__LINE__",          [this]() { return this->__LINE(); } }
    };
}


std::tm ProcessingVariables::getLocalTime() const {
    std::time_t t = std::chrono::system_clock::to_time_t(this->current_time);
    return *std::localtime(&t);
}


std::string ProcessingVariables::__DATE() const {
    auto today = std::chrono::floor<std::chrono::days>(this->current_time);
    std::chrono::year_month_day ymd{today};
    return std::format("{:04}-{:02}-{:02}", int(ymd.year()), unsigned(ymd.month()), unsigned(ymd.day()));
}

std::string ProcessingVariables::__TIME() const {
        std::time_t t = std::chrono::system_clock::to_time_t(this->current_time);
        std::tm* now = std::localtime(&t);
        return std::format("{:02}:{:02}:{:02}", now->tm_hour, now->tm_min, now->tm_sec);
}

std::string ProcessingVariables::__DATETIME() const {
        std::time_t t = std::chrono::system_clock::to_time_t(this->current_time);
        std::tm* local = std::localtime(&t);
        return std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
                local->tm_year + 1900,
                local->tm_mon + 1,
                local->tm_mday,
                local->tm_hour,
                local->tm_min,
                local->tm_sec);
}


std::string ProcessingVariables::__LINE() const {

        return "";
}


std::string ProcessingVariables::__VERSION() const {
        return VERSION;
}


std::string ProcessingVariables::__YEAR() const {
    return std::to_string(1900 + getLocalTime().tm_year);
}

std::string ProcessingVariables::__MONTH() const {
    return std::format("{:02}", getLocalTime().tm_mon + 1);
}

std::string ProcessingVariables::__DAY() const {
    return std::format("{:02}", getLocalTime().tm_mday);
}

std::string ProcessingVariables::__HOUR() const {
    return std::format("{:02}", getLocalTime().tm_hour);
}

std::string ProcessingVariables::__MINUTE() const {
    return std::format("{:02}", getLocalTime().tm_min);
}

std::string ProcessingVariables::__SECOND() const {
    return std::format("{:02}", getLocalTime().tm_sec);
}

std::string ProcessingVariables::__UNIXTIMESTAMP() const {
    std::time_t t = std::chrono::system_clock::to_time_t(this->current_time);
    return std::to_string(t);
}


std::string ProcessingVariables::__USER() const {
        const char* user = std::getenv("USER");
        if (user) {
            return std::string(user);
        }
        user = std::getenv("USERNAME");
        if (user) {
            return std::string(user);
        }
        return "";
}


std::string ProcessingVariables::__HOST() const {
        const char* host = std::getenv("HOST");
        if (host) {
            return std::string(host);
        }
        host = std::getenv("COMPUTERNAME");
        if (host) {
            return std::string(host);
        }
        return "";
}


std::string ProcessingVariables::__PWD() const {
        return std::getenv("PWD");
}


std::string ProcessingVariables::__FILE() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
            return context.inputs[0];
        }
        return "";
}


std::string ProcessingVariables::__FILE_NAME() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
            std::filesystem::path input_path = context.inputs[0];
            return input_path.filename().string();
        }
        return "";
}


std::string ProcessingVariables::__FILE_PATH() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
                std::filesystem::path input_path = std::filesystem::absolute(context.inputs[0]);
                return input_path.parent_path().string();
        }
        return "";
}


std::string ProcessingVariables::__FILE_EXT() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
            std::filesystem::path input_path = context.inputs[0];
            return input_path.extension().string();
        }
        return "";
}


std::string ProcessingVariables::__FILE_SIZE() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
            std::filesystem::path input_path = context.inputs[0];
            if (std::filesystem::exists(input_path)) {
                return std::to_string(std::filesystem::file_size(input_path));
            }
        }
        return "";
}


std::string ProcessingVariables::__FILE_CREATED() const {
        if (context.action_type == ActionType::FILE_IN_FILE_OUT ||
            context.action_type == ActionType::FILE_IN_STDOUT) {
            std::filesystem::path input_path = context.inputs[0];
            if (std::filesystem::exists(input_path)) {
                auto ftime = std::filesystem::last_write_time(input_path);
                auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime - std::filesystem::file_time_type::clock::now() + std::chrono::system_clock::now());
                std::time_t ctime = std::chrono::system_clock::to_time_t(sctp);
                return std::ctime(&ctime);
            }
        }
        return "";
}



bool ProcessingVariables::is_valid(const std::string& key) const {
        return key.starts_with("__") && key.ends_with("__");
}


std::string ProcessingVariables::get_value(const std::string& key) const {
        auto it = functionality.find(key);
        if (it != functionality.end()) {
                return it->second();
        }
        return "";
}


}
