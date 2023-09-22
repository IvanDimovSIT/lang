#pragma once
#include <string>
#include "../token/Token.h"

class StringUtil{
public:
    static bool isValidIdentifierName(const std::string& name);
    
    static int findStringLiteralEndIndex(const std::string& source, int start); 

    static int findNextEndline(const std::string& str, int start);

    static bool isSeparator(char c);

    static std::string getLine(const std::string& str, int position, int& substringStartPosition);

    static std::string getLine(const std::string& str, int position);

    static void convertValueToString(const Value& value, std::string& dest);

    static const int STRING_END_NOT_FOUND = -1;
};
