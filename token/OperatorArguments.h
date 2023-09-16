#pragma once
#include "TokenId.h"

class OperatorArguments{
public:
    static bool getArgs(TokenId id, bool& hasLeft, bool& hasRight);
};