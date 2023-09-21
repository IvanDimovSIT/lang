#pragma once
#include <vector>
#include "../token/Token.h"

class TokenSubArrayFinder{
public:
    //inclusive indexes
    //doesn't clear dest
    static void findSubArray(const std::vector<Token>& src, std::vector<const Token*>& dest, int start, int end);

    static void findSubArray(const std::vector<const Token*>& src, std::vector<const Token*>& dest, int start, int end);

    //return new position(moves forwards) (currentPosition token is ignored)
    static int findClosingCurly(const std::vector<const Token*> &tokens, int currentPosition);

    static int findClosingCurly(const std::vector<Token> &tokens, int currentPosition);

    static int findClosingParenthesis(const std::vector<const Token*> &tokens, int currentPosition);

    static int findStatementEnd(const std::vector<const Token*> &tokens, int currentPosition);

    //return the index of the first token that matches the id in the current line (empty lines following are currentPosition are ignored) (moves forwards)
    static int findFirstTokenIdInLine(const std::vector<const Token*> &tokens, int currentPosition, TokenId id);

    static int findFirstTokenId(const std::vector<const Token*> &tokens, int currentPosition, TokenId id);

    static const int TOKEN_INDEX_NOT_FOUND = -1;
};
