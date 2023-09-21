#pragma once
#include <vector>
#include "../token/Token.h"

struct Function{
    std::vector<const Token*> body;
    bool hasLeft;
    bool hasRight;
};
