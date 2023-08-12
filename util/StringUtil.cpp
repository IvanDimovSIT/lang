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
