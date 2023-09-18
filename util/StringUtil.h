#pragma once
#include <string>

class StringUtil{
public:
    static bool isValidIdentifierName(const std::string& name);
    
    static int findStringLiteralEndIndex(const std::string& source, int start); 

    static int findNextEndline(const std::string& str, int start);

    static bool isSeparator(char c);

    static std::string getLine(const std::string& str, int position);

    static const int STRING_END_NOT_FOUND = -1;
};
