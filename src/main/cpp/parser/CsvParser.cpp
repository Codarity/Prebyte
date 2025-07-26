#include "parser/CsvParser.h"

#ifdef _WIN32


#endif

namespace prebyte {

Data CsvParser::parse(const std::filesystem::path& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filepath.string());
    }

    std::string line;
    std::vector<std::string> headers;

    Data result = Data::Array{};

    if (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            headers.push_back(cell);
        }
        if (headers.empty()) {
            throw std::runtime_error("No headers found in first line of CSV file");
        }
    } else {
        throw std::runtime_error("CSV file is empty");
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cell;
        size_t index = 0;

        Data row = Data::Map{};

        while (std::getline(ss, cell, ',')) {
            if (index >= headers.size()) break;

            const std::string& key = headers[index];

            if (cell == "true" || cell == "false") {
                row[key] = (cell == "true");
            } else if (cell.find('.') != std::string::npos) {
                try {
                    row[key] = std::stod(cell);
                } catch (...) {
                    row[key] = cell;
                }
            } else {
                try {
                    row[key] = std::stoi(cell);
                } catch (...) {
                    row[key] = cell;
                }
            }

            ++index;
        }

        Data::Array arr = result.as_array();
        arr.push_back(row);
    }

    return result;
}

bool CsvParser::can_parse(const std::filesystem::path& filepath) const {
    if (filepath.extension() != ".csv") return false;

    std::ifstream file(filepath);
    if (!file.is_open()) return false;

    std::string line;
    if (!std::getline(file, line) || line.empty()) return false;

    size_t expected_columns = std::count(line.begin(), line.end(), ',') + 1;
    if (expected_columns == 0) return false;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t columns = std::count(line.begin(), line.end(), ',') + 1;
        if (columns != expected_columns) {
            return false;
        }
    }

    return true;
}

Data CsvParser::parse_string(const std::string& input) {
    std::istringstream stream(input);
    std::string line;
    std::vector<std::string> headers;

    Data result = Data::Array{};

    if (std::getline(stream, line)) {
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) {
            headers.push_back(cell);
        }
        if (headers.empty()) {
            throw std::runtime_error("No headers found in first line of CSV string");
        }
    } else {
        throw std::runtime_error("CSV string is empty");
    }

    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cell;
        size_t index = 0;

        Data row = Data::Map{};

        while (std::getline(ss, cell, ',')) {
            if (index >= headers.size()) break;

            const std::string& key = headers[index];

            if (cell == "true" || cell == "false") {
                row[key] = (cell == "true");
            } else if (cell.find('.') != std::string::npos) {
                try {
                    row[key] = std::stod(cell);
                } catch (...) {
                    row[key] = cell;
                }
            } else {
                try {
                    row[key] = std::stoi(cell);
                } catch (...) {
                    row[key] = cell;
                }
            }

            ++index;
        }

        Data::Array arr = result.as_array();
        arr.push_back(row);
    }

    return result;
}

} // namespace prebyte
