#include "TokenSubArrayFinder.h"

void TokenSubArrayFinder::findSubArray(std::vector<Token>& src, std::vector<Token*>& dest, int start, int end)
{
    for(int i=start; i<=end; i++){
        dest.push_back(&(src[i]));
    }
}

void TokenSubArrayFinder::findSubArray(std::vector<Token*>& src, std::vector<Token*>& dest, int start, int end)
{
    for(int i=start; i<=end; i++){
        dest.push_back(src[i]);
    }
}

int TokenSubArrayFinder::findClosingCurly(std::vector<Token*> &tokens, int currentPosition)
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

int TokenSubArrayFinder::findClosingParenthesis(std::vector<Token*> &tokens, int currentPosition)
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


int TokenSubArrayFinder::findClosingCurly(std::vector<Token> &tokens, int currentPosition)
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

int TokenSubArrayFinder::findStatementEnd(std::vector<Token*> &tokens, int currentPosition)
{
    const int size = tokens.size();
    for(; currentPosition < size; currentPosition++){
        if(tokens[currentPosition]->id == TokenIdEndLine)
            return currentPosition;
    }

    return size-1;
}

int TokenSubArrayFinder::findFirstTokenIdInLine(std::vector<Token*> &tokens, int currentPosition, TokenId id)
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

int TokenSubArrayFinder::findFirstTokenId(std::vector<Token*> &tokens, int currentPosition, TokenId id)
{
    for(const int size = tokens.size(); currentPosition<size; currentPosition++){
        if(tokens[currentPosition]->id == id)
            return currentPosition;
    }

    return TOKEN_INDEX_NOT_FOUND;
}

