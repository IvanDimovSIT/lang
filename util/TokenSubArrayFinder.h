#pragma once
#include <vector>
#include "../token/Token.h"

#define TOKEN_INDEX_NOT_FOUND (-1)

class TokenSubArrayFinder{
public:
    //inclusive indexes
    //doesn't clear dest
    static void findSubArray(std::vector<Token>& src, std::vector<Token*>& dest, int start, int end);
    static void findSubArray(std::vector<Token*>& src, std::vector<Token*>& dest, int start, int end);

    //return new position(moves forwards) (currentPosition token is ignored)
    static int findClosingCurly(std::vector<Token*> &tokens, int currentPosition);
    static int findClosingCurly(std::vector<Token> &tokens, int currentPosition);

    //return new position(moves backwards) (currentPosition token is ignored)
    static int findOpenCurly(std::vector<Token*> &tokens, int currentPosition);
    static int findOpenCurly(std::vector<Token> &tokens, int currentPosition);


    static int findClosingParenthesis(std::vector<Token*> &tokens, int currentPosition);
    static int findStatementEnd(std::vector<Token*> &tokens, int currentPosition);

    //return the index of the first token that matches the id in the current line (empty lines following are currentPosition are ignored) (moves forwards)
    static int findFirstTokenIdInLine(std::vector<Token*> &tokens, int currentPosition, TokenId id);

    static int findFirstTokenId(std::vector<Token*> &tokens, int currentPosition, TokenId id);
};
