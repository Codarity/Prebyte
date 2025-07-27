#pragma once

#include <string>
#include <vector>
#include <map>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace prebyte {

class Prebyte {
public:
        Prebyte(std::string settings_file);
        Prebyte();
        ~Prebyte() = default;
        void set_variable(const std::string& name, const std::string& value);
        void set_variable(const std::string& name);
        void set_variable(const std::string& name, std::vector<std::string> values);
        void set_profile(const std::string& profile_name);
        void set_ignore(const std::string& ignore_item);
        void set_rule(const std::string& rule_name, const std::string& rule_value);

        std::string process(const std::string& input);
        std::string process_file(const std::string& file_path);

        void process(const std::string& input, const std::string& output_path);
        void process_file(const std::string& file_path, const std::string& output_path);

private:
        void set_logger();
        std::string expand_tilde(const std::string& path);
};

}
