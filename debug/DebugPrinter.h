#pragma once
#include "../token/Token.h"

class DebugPrinter{
public:
    static void printTokens(const std::vector<Token>& tokens);
    static void printTokens(const std::vector<Token*>& tokens);
};