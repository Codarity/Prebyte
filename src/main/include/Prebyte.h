#pragma once
#include <string>
#include <unordered_map>

class Prebyte {
public:
    Prebyte();

    // Benutzerdefinierte Variablen setzen
    void define(const std::string& key, const std::string& value);

    // Einen String mit %%TOKEN%%-Platzhaltern verarbeiten
    std::string process_string(const std::string& input);

    // Eine Datei lesen, Platzhalter ersetzen und speichern
    bool process_file(const std::string& input_path, const std::string& output_path);

private:
    std::unordered_map<std::string, std::string> variables_;

    std::string resolve_tokens(const std::string& line);
    std::string evaluate_token(const std::string& token);
};
