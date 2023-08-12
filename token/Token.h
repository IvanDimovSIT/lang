#pragma once
#include "TokenId.h"
#include <string>
#include <vector>

struct Token{
    std::string str;
    std::vector<double> val;
    TokenId id;
};
