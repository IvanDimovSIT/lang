#pragma once
#include <string>

#define STRING_END_NOT_FOUND -1

class StringUtil{
public:
    static bool isValidIdentifierName(const std::string& name);
    static int findStringLiteralEndIndex(const std::string& source, int start); 
};
