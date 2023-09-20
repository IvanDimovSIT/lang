#include "TokenSubArrayFinder.h"

void TokenSubArrayFinder::findSubArray(const std::vector<Token>& src, std::vector<Token*>& dest, int start, int end)
{
    for(int i=start; i<=end; i++){
        dest.push_back((Token*)&(src[i]));
    }
}

void TokenSubArrayFinder::findSubArray(const std::vector<Token*>& src, std::vector<Token*>& dest, int start, int end)
{
    for(int i=start; i<=end; i++){
        dest.push_back(src[i]);
    }
}

int TokenSubArrayFinder::findClosingCurly(const std::vector<Token*> &tokens, int currentPosition)
{
    const int size = tokens.size();
    int open = 1;
    for(currentPosition++; currentPosition<size; currentPosition++){
        if(tokens[currentPosition]->id == TokenIdOpenCurly){
            open++;
        }else if(tokens[currentPosition]->id == TokenIdCloseCurly){
            open--;
            if(open == 0)
                return currentPosition;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}

int TokenSubArrayFinder::findClosingParenthesis(const std::vector<Token*> &tokens, int currentPosition)
{
    const int size = tokens.size();
    int open = 1;
    for(currentPosition++; currentPosition<size; currentPosition++){
        if(tokens[currentPosition]->id == TokenIdOpenParenthesis){
            open++;
        }else if(tokens[currentPosition]->id == TokenIdCloseParenthesis){
            open--;
            if(open == 0)
                return currentPosition;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}


int TokenSubArrayFinder::findClosingCurly(const std::vector<Token> &tokens, int currentPosition)
{
    const int size = tokens.size();
    int open = 1;
    for(currentPosition++; currentPosition<size; currentPosition++){
        if(tokens[currentPosition].id == TokenIdOpenCurly){
            open++;
        }else if(tokens[currentPosition].id == TokenIdCloseCurly){
            open--;
            if(open == 0)
                return currentPosition;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}

int TokenSubArrayFinder::findStatementEnd(const std::vector<Token*> &tokens, int currentPosition)
{
    const int size = tokens.size();
    for(; currentPosition < size; currentPosition++){
        if(tokens[currentPosition]->id == TokenIdEndLine)
            return currentPosition;
    }

    return size-1;
}

int TokenSubArrayFinder::findFirstTokenIdInLine(const std::vector<Token*> &tokens, int currentPosition, TokenId id)
{
    const int size = tokens.size();
    for(bool seenEndLine = false; currentPosition<size; currentPosition++){
        if(tokens[currentPosition]->id == id)
            return currentPosition;
        else if(tokens[currentPosition]->id == TokenIdEndLine)
            seenEndLine = true;
        else if(seenEndLine)
            return TOKEN_INDEX_NOT_FOUND;
    }

    return TOKEN_INDEX_NOT_FOUND;
}

int TokenSubArrayFinder::findFirstTokenId(const std::vector<Token*> &tokens, int currentPosition, TokenId id)
{
    for(const int size = tokens.size(); currentPosition<size; currentPosition++){
        if(tokens[currentPosition]->id == id)
            return currentPosition;
    }

    return TOKEN_INDEX_NOT_FOUND;
}

