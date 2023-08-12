#pragma once
#include <vector>
#include "../token/Token.h"

struct Function{
    std::vector<Token*> body;
    bool left;
    bool right;
};
