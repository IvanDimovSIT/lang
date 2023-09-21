#pragma once
#include <vector>
#include "../token/Token.h"

class FunctionExtractor{
public:
    static bool extractFunctions(const std::vector<Token> &tokens, std::vector<const Token*> &dest);
};
