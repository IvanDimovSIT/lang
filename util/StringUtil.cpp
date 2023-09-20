#include "StringUtil.h"

bool StringUtil::isValidIdentifierName(const std::string& name){
    const int strLen = name.size();
    if(strLen <=0 || (!std::isupper(name[0])))
        return false;

    for(int i=1; i<strLen; i++){
        if(!(std::isupper(name[i]) || std::isdigit(name[i])))
            return false;
    }

    return true;
}

int StringUtil::findStringLiteralEndIndex(const std::string& source, int start)
{
    const int size = source.size();
    for(int i=start+1; i<size; i++){
        if(source[i] == '"' && (source[i-1] != '\\' || (i-2>start && source[i-2] == '\\')))
            return i;
        else if(source[i] == '\n')
            return STRING_END_NOT_FOUND;
    }

    return STRING_END_NOT_FOUND;
}

int StringUtil::findNextEndline(const std::string& str, int start)
{
    const int size = str.size();
    for(start++; start<size; start++){
        if(str[start] == '\n')
            return start;
    }

    return STRING_END_NOT_FOUND;
}

bool StringUtil::isSeparator(char c)
{
    return c == ' ' || c == '\t';
}

std::string StringUtil::getLine(const std::string& str, int position, int& substringStartPosition)
{
    const int size = str.size();
    std::string line = "";
    while(position >= 0 && str[position] != ';' && str[position] != '\n')
        position--;

    for(substringStartPosition = ++position; position<size && str[position] != ';' && str[position] != '\n'; position++)
        line += str[position];

    return line;
}

std::string StringUtil::getLine(const std::string& str, int position)
{
    const int size = str.size();
    std::string line = "";
    while(position >= 0 && str[position] != ';' && str[position] != '\n')
        position--;

    for(++position; position<size && str[position] != ';' && str[position] != '\n'; position++)
        line += str[position];

    return line;
}
