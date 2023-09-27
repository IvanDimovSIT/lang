#pragma once
#include "TokenId.h"
#include <map>
#include <string>
#include <vector>

struct OperatorsInformation{
    std::string description;
    std::vector<std::string> examples;
};

class OperatorsHelp{
public:
    static const std::map<TokenId, OperatorsInformation> descriptions;
};
