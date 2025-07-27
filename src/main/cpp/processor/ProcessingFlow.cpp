#include "processor/ProcessingFlow.h"

namespace prebyte {

std::string ProcessingFlow::_SET_VAR(const std::string& action) {
        if (action.empty()) return "";

        size_t equal_pos = action.find('=');
        if (equal_pos == std::string::npos) {
                this->context->variables[action] = {""};
        } else {
                std::string var_name = action.substr(0, equal_pos);
                std::string var_value = action.substr(equal_pos + 1);
                this->context->variables[var_name] = {var_value};
        }
        return "";
}
std::string ProcessingFlow::_UNSET_VAR(const std::string& action) {
        if (!action.empty()) {
                this->context->variables.erase(action);
        }
        return "";
}
std::string ProcessingFlow::_SET_RULE(const std::string& action) {
        if (action.empty()) return "";
        size_t equal_pos = action.find('=');
        if (equal_pos == std::string::npos) {
                context->console_sink->set_level(this->context->rules.add_rule(action, Data()));
        } else {
                std::string rule_name = action.substr(0, equal_pos);
                std::string rule_value = action.substr(equal_pos + 1);
                context->console_sink->set_level(this->context->rules.add_rule(rule_name, Data(rule_value)));
        }
        return "";
}
std::string ProcessingFlow::_DEFINE_PROFILE(const std::string& action) {
        if (action.empty()) return "";

        std::string profile_name = action;
        if (this->context->profiles.find(profile_name) == this->context->profiles.end()) {
                this->context->profiles[profile_name] = *new Profile(profile_name);
        }
        this->flow_state = FlowState::DEFINE_PROFILE;

        return profile_name;
}
std::string ProcessingFlow::_SET_PROFILE(const std::string& action) {
        Profile profile = context->profiles[action];
        for (const auto& [key,value] : profile.get_variables()) {
                context->variables[key] = {value};
        }
        for (const auto& ignore_item : profile.get_ignore()) {
                context->ignore.insert(ignore_item);
        }
        for (const auto& [rule_name, rule_value] : profile.get_rules()) {
                context->console_sink->set_level(context->rules.add_rule(rule_name, Data(rule_value)));
        }
        return "";
}
std::string ProcessingFlow::_UNSET_PROFILE(const std::string& action) {

        return "";
}
std::string ProcessingFlow::_SET_IGNORE(const std::string& action) {
        if (action.empty()) return "";
        std::string ignore_item = action;
        this->context->ignore.insert(ignore_item);
        return "";
}
std::string ProcessingFlow::_UNSET_IGNORE(const std::string& action) {
        if (!action.empty()) {
                this->context->ignore.erase(action);
        }
        return "";
}
std::string ProcessingFlow::_DEFINE_MACRO(const std::string& action) {
        this->flow_state = FlowState::DEFINE_MACRO;
        if (action.empty()) throw std::runtime_error("Macro name cannot be empty");
        return action;
}
std::string ProcessingFlow::_EXECUTE_MACRO(const std::string& action) {
        this->flow_state = FlowState::EXECUTE_MACRO;
        if (action.empty()) throw std::runtime_error("Macro name cannot be empty");
        return action;
}
std::string ProcessingFlow::_IF(const std::string& action) {
        bool condition_result = this->is_true(action);
        this->flow_state = FlowState::IF;
        return condition_result ? "true" : "false";
}
std::string ProcessingFlow::_ELSE_IF(const std::string& action) {
        bool condition_result = this->is_true(action);
        this->flow_state = FlowState::ELSE_IF;
        return condition_result ? "true" : "false";
}
std::string ProcessingFlow::_ELSE(const std::string& action) {
        this->flow_state = FlowState::ELSE;
        return "";
}
std::string ProcessingFlow::_FOR(const std::string& action) {
        if (action.empty()) throw std::runtime_error("For loop variable cannot be empty");
        this->flow_state = FlowState::FOR;
        return action;
}

std::string ProcessingFlow::_INCLUDE(const std::string& action) {
        if (action.empty()) return "";
        std::filesystem::path include_path = action;
        if (std::filesystem::exists(include_path)) {
                return std::filesystem::absolute(include_path).string();
        }
        include_path = std::filesystem::path(this->context->rules.include_path.value()) / action;
        if (std::filesystem::exists(include_path)) {
                return std::filesystem::absolute(include_path).string();
        }
        return "";
}



bool ProcessingFlow::is_valid(const std::string& action) const {
        return action.find(" ") != std::string::npos || action == "endif" ||
                action == "else" || action == "endfor" || action == "enddef" ||
                action == "endmacro" || action == "endprofile";
}


std::string ProcessingFlow::get_value(const std::string& action) {
        std::string sub_action;
        this->flow_state = FlowState::NONE;
        this->context->logger->trace("Processing action: {}", action);
        if (action.starts_with("set ")) {
                this->context->logger->trace("Action is a 'set' command");
                sub_action = action.substr(4);
                if (sub_action.starts_with("var ")) {
                        this->context->logger->trace("Action is setting a variable");
                        return _SET_VAR(sub_action.substr(4));
                } else if (sub_action.starts_with("rule ")) {
                        this->context->logger->trace("Action is setting a rule");
                        return _SET_RULE(sub_action.substr(5));
                } else if (sub_action.starts_with("profile ")) {
                        this->context->logger->trace("Action is setting a profile");
                        return _SET_PROFILE(sub_action.substr(8));
                } else if (sub_action.starts_with("ignore ")) {
                        this->context->logger->trace("Action is setting an ignore item");
                        return _SET_IGNORE(sub_action.substr(7));
                } else if (sub_action.starts_with("igno ")) {
                        this->context->logger->trace("Action is setting an ignore item");
                        return _SET_IGNORE(sub_action.substr(5));
                }
        } else if (action.starts_with("unset ")) {
                this->context->logger->trace("Action is an 'unset' command");
                sub_action = action.substr(6);
                if (sub_action.starts_with("var ")) {
                        this->context->logger->trace("Action is unsetting a variable");
                        return _UNSET_VAR(sub_action.substr(4));
                } else if (sub_action.starts_with("ignore ")) {
                        this->context->logger->trace("Action is unsetting an ignore item");
                        return _UNSET_IGNORE(sub_action.substr(7));
                }
        } else if (action.starts_with("define ") || action.starts_with("def ")) {
                this->context->logger->trace("Action is a 'define' command");
                if (action.starts_with("define ")) sub_action = action.substr(7);
                else if (action.starts_with("def ")) sub_action = action.substr(4);
                if (sub_action.starts_with("macro ")) {
                        this->context->logger->trace("Action is defining a macro");
                        return _DEFINE_MACRO(sub_action.substr(6));
                } else if (sub_action.starts_with("profile ")) {
                        this->context->logger->trace("Action is defining a profile");
                        return _DEFINE_PROFILE(sub_action.substr(8));
                }
        } else if (action.starts_with("exec ")) {
                this->context->logger->trace("Action is executing a macro");
                return _EXECUTE_MACRO(action.substr(5));
        } else if (action.starts_with("if ")) {
                this->context->logger->trace("Action is an 'if' condition");
                return _IF(action.substr(3));
        } else if (action.starts_with("elif ")) {
                this->context->logger->trace("Action is an 'elif' condition");
                return _ELSE_IF(action.substr(8));
        } else if (action == "else") {
                this->context->logger->trace("Action is an 'else' condition");
                return _ELSE("");
        } else if (action.starts_with("for ")) {
                this->context->logger->trace("Action is a 'for' loop");
                return _FOR(action.substr(4));
        } else if (action == "endfor") {
                this->context->logger->trace("Action is an for loop end");
                this->flow_state = FlowState::END_FOR;
                return "";
        } else if (action == "endif") {
                this->context->logger->trace("Action is an condition end");
                this->flow_state = FlowState::END_IF;
                return "";
        } else if (action == "enddef") {
                this->context->logger->trace("Action is an macro definition end");
                this->flow_state = FlowState::END_DEFINE;
                return "";
        } else if (action.starts_with("include ")) {
                this->context->logger->trace("Action is an include command");
                this->flow_state = FlowState::INCLUDE;
                return _INCLUDE(action.substr(8));
        }
        return action;
}


FlowState ProcessingFlow::get_flow_state() const {
        return this->flow_state;
}

bool ProcessingFlow::is_true(const std::string& expr) const {
    return eval_or(expr);
}

bool ProcessingFlow::eval_or(const std::string& expr) const {
    size_t pos = 0;
    int parens = 0;
    for (size_t i = 0; i + 1 < expr.size(); ++i) {
        if (expr[i] == '(') parens++;
        if (expr[i] == ')') parens--;
        if (parens == 0 && expr[i] == '|' && expr[i+1] == '|') {
            std::string left = expr.substr(0, i);
            std::string right = expr.substr(i + 2);
            return is_true(trim(left)) || is_true(trim(right));
        }
    }
    return eval_and(expr);
}

bool ProcessingFlow::eval_and(const std::string& expr) const {
    size_t pos = 0;
    int parens = 0;
    for (size_t i = 0; i + 1 < expr.size(); ++i) {
        if (expr[i] == '(') parens++;
        if (expr[i] == ')') parens--;
        if (parens == 0 && expr[i] == '&' && expr[i+1] == '&') {
            std::string left = expr.substr(0, i);
            std::string right = expr.substr(i + 2);
            return is_true(trim(left)) && is_true(trim(right));
        }
    }
    return eval_not(expr);
}

bool ProcessingFlow::eval_not(const std::string& expr) const {
    std::string trimmed = trim(expr);
    if (trimmed.starts_with('!')) {
        return !is_true(trimmed.substr(1));
    }
    if (trimmed.starts_with('(') && trimmed.ends_with(')')) {
        return is_true(trimmed.substr(1, trimmed.size() - 2));
    }
    return eval_comparison(trimmed);
}

bool ProcessingFlow::eval_comparison(const std::string& expr) const {
    static const std::regex cmp_regex(R"(^\s*([a-zA-Z][a-zA-Z0-9_]*|".*")\s*(==|!=)\s*([a-zA-Z][a-zA-Z0-9_]*|".*")\s*$)");
    std::smatch match;
    if (std::regex_match(expr, match, cmp_regex)) {
        std::string var = get_str_value(match[1]);
        std::string op = match[2];
        std::string val = get_str_value(match[3]);

        if (op == "==") return var == val;
        if (op == "!=") return var != val;
    }

    return this->context->variables.find(expr) != this->context->variables.end();
}

std::string ProcessingFlow::get_str_value(const std::string& token) const {
    std::string t = trim(token);
    if (t.starts_with('"') && t.ends_with('"')) {
        return t.substr(1, t.size() - 2);
    } else if (t.ends_with("]")) {
            std::string var_name = t.substr(0, t.find('['));
            std::cout << "Teste Variable" << var_name << std::endl;
            int index = std::stoi(t.substr(t.find('[') + 1, t.find(']') - t.find('[') - 1));
            std::cout << "Teste Index" << index << std::endl;
            return this->context->variables[var_name][index];
    } else {
        auto it = this->context->variables.find(t);
        return it != this->context->variables.end() ? it->second[0] : "";
    }
}

std::string ProcessingFlow::trim(const std::string& s) const {
    size_t start = s.find_first_not_of(" \t");
    size_t end = s.find_last_not_of(" \t");
    return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
}


bool ProcessingFlow::is_skipable(const std::string& action, bool ignore) const {
        return ignore && !action.starts_with("if ") && !action.starts_with("elif ") && !action.starts_with("else");
}



}
