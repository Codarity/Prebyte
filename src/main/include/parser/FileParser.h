#pragma once

#include <filesystem>
#include <string>
#include "datatypes/Data.h"

namespace prebyte {

class FileParser {
public:
    virtual ~FileParser() = default;

    virtual Data parse(const std::filesystem::path& filepath) = 0;

    virtual bool can_parse(const std::filesystem::path& filepath) const = 0;
};

}
