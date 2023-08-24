#include "StringUtil.h"

bool StringUtil::isValidIdentifierName(const std::string& name){
    const int strLen = name.size();
    if(strLen <=0 || name[0] < 'A' || name[0] > 'Z')
        return false;

    for(int i=1; i<strLen; i++){
        if(
            !(
                ( (name[i] >= 'A') && (name[i] <= 'Z') ) ||
                ( (name[i] >= '0') && (name[i] <= '9') )
            )
        )
            return false;
    }

    return true;
}

int StringUtil::findStringLiteralEndIndex(const std::string& source, int start)
{
    const int size = source.size();
    for(int i=start+1; i<size; i++){
        if(source[i] == '"' && source[i-1] != '\\')
            return i;
        else if(source[i] == '\n')
            return STRING_END_NOT_FOUND;
    }

    return STRING_END_NOT_FOUND;
}
