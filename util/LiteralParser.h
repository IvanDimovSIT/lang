#pragma once
#include <string>
#include <vector>

class LiteralParser{
public:
    static bool parse(const std::string& literal, std::vector<double>& value);
private:
    static bool isValidChar(char c);
    static bool parseSingleValue(const std::string literal, double& value);
};
