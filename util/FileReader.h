#pragma once
#include <string>

class FileReader{
public:
    static bool read(const std::string& filepath, std::string& contents);
};
