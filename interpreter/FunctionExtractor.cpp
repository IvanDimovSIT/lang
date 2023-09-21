#include "FunctionExtractor.h"
#include "../util/TokenSubArrayFinder.h"

bool FunctionExtractor::extractFunctions(const std::vector<Token> &tokens, std::vector<const Token*> &dest)
{
    dest.clear();
    const int size = tokens.size();
    for(int i=0; i<size; i++){
        if(tokens[i].id != TokenIdFunctionDeclaration){
            dest.push_back(&tokens[i]);
        }else{
            while(tokens[i].id != TokenIdOpenCurly && i<size)
                i++;
            
            if(i >= size)
                return false;

            i = TokenSubArrayFinder::findClosingCurly(tokens, i);
            if(i == TokenSubArrayFinder::TOKEN_INDEX_NOT_FOUND)
                return false;
        }
    }

    return true;
}