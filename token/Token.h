#pragma once
#include "TokenId.h"
#include <string>
#include <vector>

typedef std::vector<double> Value;

struct Token{
    std::string str;
    Value val;
    TokenId id;
};
