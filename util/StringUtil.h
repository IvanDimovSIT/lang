#pragma once
#include <string>

class StringUtil{
public:
    static bool isValidIdentifierName(const std::string& name);
    
    static int findStringLiteralEndIndex(const std::string& source, int start); 
    
    static const int STRING_END_NOT_FOUND = -1;
};
