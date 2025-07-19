#include "datatypes/Data.h"

namespace prebyte {

const std::string& Data::as_string() const {
    if (!is_string()) throw std::bad_variant_access();
    return std::get<std::string>(value);
}

int Data::as_int() const {
    if (!is_int()) throw std::bad_variant_access();
    return std::get<int>(value);
}

double Data::as_double() const {
    if (!is_double()) throw std::bad_variant_access();
    return std::get<double>(value);
}

bool Data::as_bool() const {
    if (!is_bool()) throw std::bad_variant_access();
    return std::get<bool>(value);
}

const Data::Map& Data::as_map() const {
    if (!is_map()) throw std::bad_variant_access();
    return std::get<Map>(value);
}

const Data::Array& Data::as_array() const {
    if (!is_array()) throw std::bad_variant_access();
    return std::get<Array>(value);
}

const Data& Data::operator[](const std::string& key) const {
    if (!is_map()) throw std::runtime_error("Not a map");
    const auto& m = std::get<Map>(value);
    auto it = m.find(key);
    if (it == m.end()) throw std::out_of_range("Key not found: " + key);
    return it->second;
}

Data& Data::operator[](const std::string& key) {
    if (!is_map()) throw std::runtime_error("Not a map");
    return std::get<Map>(value)[key];
}

const Data& Data::operator[](size_t index) const {
    if (!is_array()) throw std::runtime_error("Not an array");
    const auto& arr = std::get<Array>(value);
    if (index >= arr.size()) throw std::out_of_range("Index out of range");
    return arr[index];
}

Data& Data::operator[](size_t index) {
    if (!is_array()) throw std::runtime_error("Not an array");
    auto& arr = std::get<Array>(value);
    if (index >= arr.size()) throw std::out_of_range("Index out of range");
    return arr[index];
}

} // namespace prebyte
