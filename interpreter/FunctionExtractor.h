#pragma once
#include <vector>
#include "../token/Token.h"

class FunctionExtractor{
public:
    static bool extractFunctions(std::vector<Token*> &tokens, std::vector<Token*> &dest);
};