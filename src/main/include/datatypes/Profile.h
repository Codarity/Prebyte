#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include "datatypes/Data.h"
#include "Rules.h"

namespace prebyte {

/**
 * @brief Represents a named profile that contains rules, variables, and ignore lists.
 *
 * The Profile class provides a way to group related configuration data under a common name.
 * It contains a set of named variables, ignored rule names, and rule definitions in a flexible
 * `Data` format. Profiles can be used to encapsulate reusable or named configurations
 * that influence how the application behaves.
 *
 * A profile can be built incrementally by adding variables, ignore entries, and rules
 * through the provided methods.
 */
class Profile {
private:
    std::string name;  ///< The name of the profile.
    std::map<std::string, std::vector<std::string>> variables;  ///< Key-value mapping of profile variables.
    std::unordered_set<std::string> ignore;  ///< Set of rules or elements to ignore.
    std::map<std::string, Data> rules;  ///< Named rules stored as Data objects.

public:
    /** @brief Default constructor that creates an unnamed, empty profile. */
    Profile() = default;

    /**
     * @brief Constructs a profile with a given name.
     * @param name The name to assign to the profile.
     */
    Profile(const std::string& name);

    /**
     * @brief Adds a single variable and its value to the profile.
     * @param variable_name Name/key of the variable.
     * @param variable_value The value associated with the variable.
     */
    void add_variable(const std::string& variable_name, const std::string& variable_value);

    /**
     * @brief Adds multiple variables to the profile at once.
     * @param variables A map of variable names to their list of values.
     */
    void add_variable(const std::map<std::string, std::vector<std::string>>& variables);

    /**
     * @brief Adds a single item to the ignore list.
     * @param ignore_item The rule or identifier to be ignored.
     */
    void add_ignore(const std::string& ignore_item);

    /**
     * @brief Adds multiple items to the ignore list.
     * @param ignore_items A set of strings to be ignored.
     */
    void add_ignore(const std::unordered_set<std::string>& ignore_items);

    /**
     * @brief Adds a single rule by name and value.
     * @param rule_name The name of the rule.
     * @param rule_value The rule logic or expression as a string.
     */
    void add_rules(std::string rule_name, const std::string& rule_value);

    /**
     * @brief Adds multiple rules to the profile at once.
     * @param rules A map of rule names and their values as strings.
     */
    void add_rules(std::map<std::string, std::string> rules);

    /**
     * @brief Returns the name of the profile.
     * @return Reference to the profile's name.
     */
    std::string& get_name() const;

    /**
     * @brief Returns a reference to the variable map.
     * @return Reference to the map of variables.
     */
    std::map<std::string, std::vector<std::string>>& get_variables() const;

    /**
     * @brief Returns a reference to the ignore set.
     * @return Reference to the set of ignored items.
     */
    std::unordered_set<std::string>& get_ignore() const;

    /**
     * @brief Returns a reference to the map of rules.
     * @return Reference to the rules map.
     */
    std::map<std::string, Data>& get_rules() const;
};

}
