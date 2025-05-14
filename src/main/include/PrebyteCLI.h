#pragma once
#include <string>
#include <unordered_map>

struct CLI {
    bool use_stdin = false;                      // ob -s verwendet wurde
    std::string input_string;                    // falls -s verwendet
    std::string input_path;                      // sonst Pfad zur Datei
    std::string output_path;                     // optional
    std::unordered_map<std::string, std::string> defines; // -DKEY=val

    static CLI parse(int argc, char** argv);
};
