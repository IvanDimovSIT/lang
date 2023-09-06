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
    int p;
    int open = 1;
    for(p = currentPosition+1; p<size; p++){
        if(tokens[p]->id == TokenIdOpenCurly){
            open++;
        }else if(tokens[p]->id == TokenIdCloseCurly){
            open--;
            if(open == 0)
                return p;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}

int TokenSubArrayFinder::findClosingParenthesis(std::vector<Token*> &tokens, int currentPosition)
{
    const int size = tokens.size();
    int p;
    int open = 1;
    for(p = currentPosition+1; p<size; p++){
        if(tokens[p]->id == TokenIdOpenParenthesis){
            open++;
        }else if(tokens[p]->id == TokenIdCloseParenthesis){
            open--;
            if(open == 0)
                return p;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}


int TokenSubArrayFinder::findClosingCurly(std::vector<Token> &tokens, int currentPosition)
{
    const int size = tokens.size();
    int p;
    int open = 1;
    for(p = currentPosition+1; p<size; p++){
        if(tokens[p].id == TokenIdOpenCurly){
            open++;
        }else if(tokens[p].id == TokenIdCloseCurly){
            open--;
            if(open == 0)
                return p;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}

int TokenSubArrayFinder::findOpenCurly(std::vector<Token*> &tokens, int currentPosition)
{
    int p;
    int open = -1;
    for(p = currentPosition-1; p>=0; p--){
        if(tokens[p]->id == TokenIdOpenCurly){
            open++;
            if(open == 0)
                return p;
        }else if(tokens[p]->id == TokenIdCloseCurly){
            open--;
        }
    }

    return TOKEN_INDEX_NOT_FOUND;
}


int TokenSubArrayFinder::findOpenCurly(std::vector<Token> &tokens, int currentPosition)
{
    int p;
    int open = -1;
    for(p = currentPosition-1; p>=0; p--){
        if(tokens[p].id == TokenIdOpenCurly){
            open++;
            if(open == 0)
                return p;
        }else if(tokens[p].id == TokenIdCloseCurly){
            open--;
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
    bool seenEndLine = false;
    for(; currentPosition<size; currentPosition++){
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

