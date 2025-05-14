#include "Prebyte.h"
#include <fstream>
#include <sstream>
#include <regex>
#include <iostream>

Prebyte::Prebyte() {}

void Prebyte::define(const std::string& key, const std::string& value) {
    variables_[key] = value;
}

bool Prebyte::process_file(const std::string& input_path, const std::string& output_path) {
    std::ifstream in(input_path);
    if (!in.is_open()) {
        std::cerr << "[Prebyte] Failed to open input file: " << input_path << "\n";
        return false;
    }

    std::ofstream out(output_path);
    if (!out.is_open()) {
        std::cerr << "[Prebyte] Failed to open output file: " << output_path << "\n";
        return false;
    }

    std::string line;
    while (std::getline(in, line)) {
        out << resolve_tokens(line) << "\n";
    }

    return true;
}

std::string Prebyte::process_string(const std::string& input) {
    std::istringstream iss(input);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        oss << resolve_tokens(line) << "\n";
    }
    return oss.str();
}

std::string Prebyte::resolve_tokens(const std::string& line) {
    static std::regex token_pattern(R"(%%(.*?)%%)");
    std::smatch match;
    std::string output = line;
    while (std::regex_search(output, match, token_pattern)) {
        std::string token = match[1].str();
        std::string value = evaluate_token(token);
        output.replace(match.position(0), match.length(0), value);
    }
    return output;
}

std::string Prebyte::evaluate_token(const std::string& token) {
    auto it = variables_.find(token);
    if (it != variables_.end()) {
        return it->second;
    }
    return "[[UNRESOLVED:" + token + "]]";
}
