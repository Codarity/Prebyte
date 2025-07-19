#pragma once

#include <string>

namespace prebyte {

class Variable {
private:
        std::string name;
        std::string value;

public:
        Variable(const std::string& name, const std::string& value);
        Variable(const std::string& name);

        std::string getName() const;
        std::string getValue() const;
};

}
