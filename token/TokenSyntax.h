#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "../token/TokenId.h"

class TokenSyntax{
public:
    static bool isValidToken(const std::string& tokenString);

    static TokenId getToken(const std::string& tokenString);

    static bool isSingleCharToken(char token);
    
private:
    static std::unordered_map<std::string, TokenId> tokenMap;
    
    static std::unordered_set<char> singleCharTokens;
};