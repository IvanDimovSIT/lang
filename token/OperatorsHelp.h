#pragma once
#include "TokenId.h"
#include <map>
#include <string>

class OperatorsHelp{
public:
    static const std::map<TokenId, std::string> descriptions;
};
