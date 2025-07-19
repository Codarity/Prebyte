#pragma once

#include <string>
#include <variant>
#include <map>
#include <vector>
#include <stdexcept>
#include <type_traits>
#include <stdexcept>
#include <sstream>

namespace prebyte {

class Data {
public:
    using Map    = std::map<std::string, Data>;
    using Array  = std::vector<Data>;
    using Value  = std::variant<std::monostate, std::string, int, double, bool, Map, Array>;

private:
    Value value;

public:
    Data() = default;
    Data(const char* v) : value(std::string(v)) {}
    Data(std::string v) : value(std::move(v)) {}
    Data(int v)         : value(v) {}
    Data(double v)      : value(v) {}
    Data(bool v)        : value(v) {}
    Data(Map v)         : value(std::move(v)) {}
    Data(Array v)       : value(std::move(v)) {}

    bool is_null() const { return std::holds_alternative<std::monostate>(value); }
    bool is_string() const { return std::holds_alternative<std::string>(value); }
    bool is_int() const { return std::holds_alternative<int>(value); }
    bool is_double() const { return std::holds_alternative<double>(value); }
    bool is_bool() const { return std::holds_alternative<bool>(value); }
    bool is_map() const { return std::holds_alternative<Map>(value); }
    bool is_array() const { return std::holds_alternative<Array>(value); }

    const std::string& as_string() const;
    int as_int() const;
    double as_double() const;
    bool as_bool() const;
    const Map& as_map() const;
    const Array& as_array() const;

    // Map-like access
    const Data& operator[](const std::string& key) const;
    Data& operator[](const std::string& key);

    // Array-like access
    const Data& operator[](size_t index) const;
    Data& operator[](size_t index);
};

} 
