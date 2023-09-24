#pragma once
#include "TokenId.h"

class OperatorArguments{
public:
    // returns false if not an operator
    static bool getArgs(TokenId id, bool& hasLeft, bool& hasRight);
};
