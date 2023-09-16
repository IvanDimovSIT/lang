#include "TokenSyntax.h"
#include <cassert>

std::unordered_map<std::string, TokenId> TokenSyntax::tokenMap = {
    {"+", TokenIdAdd},
    {"-", TokenIdSubtract},
    {"*", TokenIdMultiply},
    {"/", TokenIdDivide},
    {"%", TokenIdMod},
    {"**",TokenIdPower},
    {"<", TokenIdLessThan},
    {">", TokenIdGreaterThan},
    {"<=",TokenIdLessThanOrEquals},
    {">=",TokenIdGreaterThanOrEquals},
    {"==",TokenIdIsEquals},
    {"!=",TokenIdNotEquals},
    {"=", TokenIdEquals},
    {"!", TokenIdLogicalNot},
    {"|", TokenIdUnion},
    {":", TokenIdSelect},
    {"?", TokenIdRandom},
    {"^", TokenIdCeil},
    {"_", TokenIdFloor},
    {"~", TokenIdRound},
    {"$", TokenIdSort},
    {"@", TokenIdReverse},
    {"&", TokenIdMakeSet},
    {"#", TokenIdCount},
    {"#+",TokenIdSumAll},
    {"#*",TokenIdMultiplyAll},
    {"f", TokenIdFunctionDeclaration},
    {"a", TokenIdLeftParam},
    {"b", TokenIdRightParam},
    {"{", TokenIdOpenCurly},
    {"}", TokenIdCloseCurly},
    {"(", TokenIdOpenParenthesis},
    {")", TokenIdCloseParenthesis},
    {"\n",TokenIdEndLine},
    {";", TokenIdEndLine},
    {"if",TokenIdIf},
    {"do",TokenIdLoop},
    {"i", TokenIdIterate},
    {"r", TokenIdRead},
    {"w", TokenIdWrite},
    {"g", TokenIdReadText},
    {"t", TokenIdWriteText},
    {"\\",TokenIdApplyToEach},
    {"<<",TokenIdLeftRotate},
    {">>",TokenIdRightRotate},
    {"->",TokenIdRemove},
    {"<-",TokenIdRemain},
    {"[", TokenIdAsyncStart},
    {"]", TokenIdAsyncEnd},
    {"!!", TokenIdAsyncJoin}
};

bool TokenSyntax::isValidToken(const std::string& tokenString)
{
    return tokenMap.find(tokenString) != tokenMap.end();
}

TokenId TokenSyntax::getToken(const std::string& tokenString)
{
    const auto found = tokenMap.find(tokenString);
    assert(found != tokenMap.end() && "Invalid token");
    return found->second;
}

bool TokenSyntax::isSingleCharToken(char token)
{
switch (token)
    {
    case 'i':
    case 'f':
    case 'a':
    case 'b':
    case 'r':
    case 'w':
    case 'g':
    case 't':
    case '{':
    case '}':
    case '(':
    case ')':
    case '\n':
    case ';':
    case '@':
    case '?':
    case '|':
    case '&':
    case ':':
    case '_':
    case '$':
    case '~':
    case '^':
    case '\\':
    case '[':
    case ']':
        return true;
    default:
        return false;
    }
}

