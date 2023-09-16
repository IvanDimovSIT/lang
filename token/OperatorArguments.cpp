#include "OperatorArguments.h"

bool OperatorArguments::getArgs(TokenId id, bool& hasLeft, bool& hasRight)
{
    switch (id)
    {
    case TokenIdCount:
    case TokenIdLogicalNot:
    case TokenIdIterate:
    case TokenIdSumAll:
    case TokenIdMultiplyAll:
    case TokenIdRandom:
    case TokenIdFloor:
    case TokenIdCeil:
    case TokenIdRound:
    case TokenIdSort:
    case TokenIdReverse:
    case TokenIdApplyToEach:
    case TokenIdMakeSet:
        hasLeft = true;
        hasRight = false;
        return true;
    break;
    case TokenIdAdd:
    case TokenIdSubtract:
    case TokenIdMultiply:
    case TokenIdDivide:
    case TokenIdMod:
    case TokenIdPower:
    case TokenIdLessThan:
    case TokenIdGreaterThan:
    case TokenIdLessThanOrEquals:
    case TokenIdGreaterThanOrEquals:
    case TokenIdIsEquals:
    case TokenIdNotEquals:
    case TokenIdUnion:
    case TokenIdSelect:
    case TokenIdLeftRotate:
    case TokenIdRightRotate:
    case TokenIdRemove:
    case TokenIdRemain:
        hasLeft = true;
        hasRight = true;
        return true;
    break;
    default:return false;
    }
}
