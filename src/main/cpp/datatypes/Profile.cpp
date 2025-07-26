#include "datatypes/Profile.h"

namespace prebyte {

Profile::Profile(const std::string& name) : name(name) {}

void Profile::add_variable(const std::string& variable_name, const std::string& variable_value) {
    variables[variable_name] = {variable_value};
}

void Profile::add_variable(const std::map<std::string, std::vector<std::string>>& variables) {
    this->variables.insert(variables.begin(), variables.end());
}

void Profile::add_ignore(const std::string& ignore_item) {
    ignore.insert(ignore_item);
}

void Profile::add_ignore(const std::unordered_set<std::string>& ignore_items) {
    ignore.insert(ignore_items.begin(), ignore_items.end());
}

void Profile::add_rules(std::string rule_name, const std::string& rule_value) {
    rules[rule_name] = rule_value;
}

void Profile::add_rules(std::map<std::string, std::string> rules) {
    this->rules.insert(rules.begin(), rules.end());
}

std::string& Profile::get_name() const {
    return const_cast<std::string&>(name);
}

std::map<std::string,std::vector<std::string>>& Profile::get_variables() const {
    return const_cast<std::map<std::string,std::vector<std::string>>&>(variables);
}

std::unordered_set<std::string>& Profile::get_ignore() const {
    return const_cast<std::unordered_set<std::string>&>(ignore);
}

std::map<std::string,Data>& Profile::get_rules() const {
    return const_cast<std::map<std::string,Data>&>(rules);
}

}
