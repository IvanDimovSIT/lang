#include "TokenSyntax.h"
#include <cassert>

std::unordered_map<std::string, TokenId> TokenSyntax::tokenMap = {
    {"+", TokenIdAdd},
    {"-", TokenIdSubtract},
    {"*", TokenIdMultiply},
    {"/", TokenIdDivide},
    {"%", TokenIdMod},
    {"**",TokenIdPower},
    {"s", TokenIdSine},
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
    {"c", TokenIdConvert},
    {"\\",TokenIdApplyToEach},
    {"<<",TokenIdLeftRotate},
    {">>",TokenIdRightRotate},
    {"->",TokenIdRemove},
    {"<-",TokenIdRemain},
    {"[", TokenIdAsyncStart},
    {"]", TokenIdAsyncEnd},
    {"!!", TokenIdAsyncJoin}
};

std::unordered_set<char> TokenSyntax::singleCharTokens = {
    'i', 'f', 'a', 'b', 'r', 'w', 'g', 't', 's', 'c', '{', '}', '(', ')', '\n',
    ';', '@', '?', '|', '&', ':', '_', '$', '~', '^', '\\', '[', ']'
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
    return singleCharTokens.find(token) != singleCharTokens.end();
}
