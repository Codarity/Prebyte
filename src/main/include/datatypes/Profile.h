#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include "datatypes/Data.h"
#include "Rules.h"

namespace prebyte {

class Profile {
private:
        std::string name;
        std::map<std::string,std::vector<std::string>> variables;
        std::unordered_set<std::string> ignore;
        std::map<std::string, Data> rules;
public:
        Profile() = default;
        Profile(const std::string& name);

        void add_variable(const std::string& variable_name, const std::string& variable_value);
        void add_variable(const std::map<std::string, std::vector<std::string>>& variables);
        void add_ignore(const std::string& ignore_item);
        void add_ignore(const std::unordered_set<std::string>& ignore_items);
        void add_rules(std::string rule_name, const std::string& rule_value);
        void add_rules(std::map<std::string, std::string> rules);
        std::string& get_name() const;
        std::map<std::string,std::vector<std::string>>& get_variables() const;
        std::unordered_set<std::string>& get_ignore() const;
        std::map<std::string,Data>& get_rules() const;
};

}
