#pragma once
#include <string>
#include <vector>
#include "../token/Token.h"

class LiteralParser{
public:
    static bool parse(const std::string& literal, Value& value);
    static bool parseString(const std::string& literal, Value& value);
private:
    static bool isValidChar(char c);
    static bool parseSingleValue(const std::string literal, double& value);
};
