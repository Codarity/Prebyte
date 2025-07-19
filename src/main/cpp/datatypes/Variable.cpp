#include "datatypes/Variable.h"

namespace prebyte {

Variable::Variable(const std::string& name, const std::string& value)
    : name(name), value(value) {}

Variable::Variable(const std::string& name)
    : name(name), value("") {}

std::string Variable::getName() const {
    return name;
}

std::string Variable::getValue() const {
    return value;
}

}
