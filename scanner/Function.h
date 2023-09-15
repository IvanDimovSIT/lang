#pragma once
#include <vector>
#include "../token/Token.h"

struct Function{
    std::vector<Token*> body;
    bool hasLeft;
    bool hasRight;
};
